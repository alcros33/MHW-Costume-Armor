#include "MainWindow.hpp"
#include "Config.h"
#include <QThread>
#include <QMovie>
#include <QInputDialog>
#include <QFileDialog>
#include <QActionGroup>

/// These file contains Member definitions of the MainWindow class
/// Related to initiliaze things before GUI is showed.

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(PROJECT_NAME);
    ui->FetchButton->setEnabled(false);
    ui->WriteButton->setEnabled(false);

    ui->toolBar->insertSeparator(ui->actionLoad_Armor_Toolbar);
    ui->toolBar->insertWidget(ui->actionLoad_Armor_Toolbar, ui->savedSetsLabel);
    ui->toolBar->insertSeparator(ui->actionLoad_Armor_Toolbar);
    ui->toolBar->insertWidget(ui->actionLoad_Armor_Toolbar, ui->savedComboBox);
    ui->toolBar->insertSeparator(ui->actionLoad_Armor_Toolbar);

    connect(ui->actionAbout, QAction::triggered, this, _aboutInfo);
    connect(ui->actionTutorial, QAction::triggered, this, _instructions);
    connect(ui->actionSafe_Mode, QAction::triggered, this, _toggleSafe);
    connect(ui->actionExit, QAction::triggered, this, close);

    connect(ui->SearchButton, QPushButton::released, this, _findAddr);
    connect(ui->FetchButton, SIGNAL(released()), this, SLOT(_fetchData()));
    connect(ui->WriteButton, QPushButton::released, this, _writeData);
    connect(ui->ClearButton, QPushButton::released, this, _clearArmor);
    connect(ui->ChangeAllButton, QPushButton::released, this, _changeAll);
    connect(ui->actionManually_Input_ID, QAction::triggered, this, _manualInputValue);

    connect(ui->actionSave_Current_Armor, QAction::triggered, this, _saveCurrentSet);
    connect(ui->actionLoad_Armor_Toolbar, QAction::triggered, this, _loadSavedSet);
    connect(ui->actionLoad_Armor, QAction::triggered, this, _loadSavedSetPopup);
    connect(ui->actionDelete_Armor, QAction::triggered, this, _deleteCurrentSet);
    connect(ui->actionChange_Steam_Path, QAction::triggered, this, _getCustomSteamPath);

    _inputBoxes[0] = ui->headEdit ;
    _inputBoxes[1] = ui->bodyEdit ;
    _inputBoxes[2] = ui->armsEdit ;
    _inputBoxes[3] = ui->waistEdit;
    _inputBoxes[4] = ui->legsEdit ;

    this->_armorDataFound = _loadConfigFiles();

    if (_Settings.find("Safe Mode") == _Settings.end())
        _Settings["Safe Mode"] = true;

    if (_Settings.find("Game Version") == _Settings.end())
        _Settings["Game Version"] = "Latest";
    
    if (_Settings.find("Language") == _Settings.end())
        _Settings["Language"] = "English";

    if (_Settings.find("Steam Path") != _Settings.end())
    {
        std::string SteamPath = _Settings["Steam Path"].get<std::string>();
        _MHManager.setSteamDirectory(SteamPath);
        ui->actionSteam_Current->setText(("Current : " + SteamPath).c_str());
    }

    _populateVersionSelector();
    _populateSavedSets();
    _populateLanguages();
    _translateArmorData();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::show()
{
    if (!this->_armorDataFound)
    {
        DEBUG_LOG(ERROR, "Armor Data json file searched at " << armorDataFile<< " and not found");
        DialogWindow *Dia = new DialogWindow(nullptr, "ERROR FATAL", "Couldn't find " +
            armorDataFile.filename() +
            "\nIf It was deleted, re-download the program.", Status::ERROR0);
        Dia->getOkButton()->setStyleSheet("");
        Dia->show();
        this->close();
    }
    else if (!this->_MHManager.processOpen())
    {
        DialogWindow *Dia = new DialogWindow(nullptr, "ERROR", "MHW is Closed\nPlease open it before starting.", Status::ERROR0);
        Dia->getOkButton()->setStyleSheet("");
        Dia->show();
        this->close();
    }
    else
    {
        QMainWindow::show();
        if (!_Settings["Safe Mode"])
        {
            this->_unsafeWarning();
            ui->actionSafe_Mode->setChecked(false);
        }
    }
}

void MainWindow::_showTestGui() {QMainWindow::show();}

bool MainWindow::_loadConfigFiles()
{
    if (settingsFile.exists())
    {
        std::ifstream i(settingsFile.str());
        i >> _Settings;
    }
    if (savedSetsFile.exists())
    {
        std::ifstream i(savedSetsFile.str());
        i >> _SavedSets;
    }
    if(!armorDataFile.exists())
        return false;
    
    std::ifstream i(armorDataFile.str());
    i >> _ArmorData;
    return true;
}

void MainWindow::_populateVersionSelector()
{
    QMenu *menuVersions = new QMenu(ui->menuOptions);
    menuVersions->setObjectName(QString::fromUtf8("menu_Versions"));
    menuVersions->setTitle("Select Game Version");
    ui->menuOptions->addAction(menuVersions->menuAction());

    _versionGroup = new QActionGroup(this);

    QFont font;
    font.setBold(true);

    _versionActions.reserve(MH_Memory::Versions.size());
    std::string TargetVersion = _Settings["Game Version"];
    for (auto it=MH_Memory::Versions.begin(); it!=MH_Memory::Versions.end(); ++it)
    {
        QAction *tmp = new QAction(this);
        connect(tmp, QAction::triggered, this, _updateSelectedVersion);
        tmp->setText(it->first.c_str());
        tmp->setCheckable(true);
        menuVersions->addAction(tmp);
        _versionGroup->addAction(tmp);
        if (it->first == TargetVersion)
        {
            tmp->setChecked(true);
            tmp->setFont(font);
        }
        _versionActions.push_back(tmp);
    }
    if (_versionGroup->checkedAction() == 0)
    {
        DEBUG_LOG(ERROR, "Encountered unknown game version " << TargetVersion << " setting it to 'Latest'");
        _versionActions.back()->setChecked(true);
        _versionActions.back()->setFont(font);
    }
}

void MainWindow::_populateLanguages()
{
    QMenu *menuLangs = new QMenu(ui->menuOptions);
    menuLangs->setObjectName(QString::fromUtf8("menu_Langs"));
    menuLangs->setTitle("Language");
    ui->menuOptions->addAction(menuLangs->menuAction());

    _langGroup = new QActionGroup(this);

    QFont font; font.setBold(true);

    std::vector<std::string> Langs;
    for (const auto &el :_ArmorData.front().items())
        if (el.key() != "Mode" && el.key() != "Danger")
            Langs.push_back(el.key());

    _langActions.reserve(Langs.size());
    std::string TargetLang = _Settings["Language"];
    for (const auto &la : Langs)
    {
        QAction *tmp = new QAction(this);
        connect(tmp, QAction::triggered, this, _translateArmorData);
        tmp->setText(la.c_str());
        tmp->setCheckable(true);
        menuLangs->addAction(tmp);
        _langGroup->addAction(tmp);
        if (la == TargetLang)
        {
            tmp->setChecked(true);
            tmp->setFont(font);
        }
        _langActions.push_back(tmp);
    }
    if (_langGroup->checkedAction() == 0)
    {
        DEBUG_LOG(ERROR, "Encountered unknown language " << TargetLang << " setting it to 'English'");
        _langActions.back()->setChecked(true);
        _langActions.back()->setFont(font);
    }
}

void MainWindow::_populateComboBoxes()
{
    int ID;
    std::string Mode;

    for (int i = 0; i < 5; ++i)
        this->_inputBoxes[i]->addItem("Nothing", 255);

    for (auto &el : _transArmorData.items())
    {
        ID = el.value()["ID"];

        if (el.value()["Danger"])
        {
            _unsafeArmors.insert(el.key());

            if (_Settings["Safe Mode"])
                continue;
        }
        Mode = el.value()["Mode"];
        
        for (int i = 0; i < 5; ++i)
        {
            if (Mode[i] == '1')
                this->_inputBoxes[i]->addItem(el.key().c_str(), ID);
        }
    }
    for (int i = 0; i < 5; ++i)
        _safeCount[i] = this->_inputBoxes[i]->count();
}