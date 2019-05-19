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

    connect(ui->actionAbout, QAction::triggered, this, _aboutInfo);
    connect(ui->actionTutorial, QAction::triggered, this, _Instructions);
    connect(ui->actionSafe_Mode, QAction::triggered, this, _ToggleSafe);
    connect(ui->actionExit, QAction::triggered, this, close);

    connect(ui->SearchButton, QPushButton::released, this, _FindAddr);
    connect(ui->FetchButton, SIGNAL(released()), this, SLOT(_FetchData()));
    connect(ui->WriteButton, QPushButton::released, this, _WriteData);
    connect(ui->ClearButton, QPushButton::released, this, _ClearArmor);
    connect(ui->ChangeAllButton, QPushButton::released, this, _ChangeAll);

    connect(ui->actionSave_Current_Armor, QAction::triggered, this, _SaveCurrentSet);
    connect(ui->actionLoad_Armor, QAction::triggered, this, _LoadSavedSet);
    connect(ui->actionChange_Steam_Path, QAction::triggered, this, _GetCustomSteamPath);

    connect(ui->actionManaged_Saved_Sets, QAction::triggered, this, _NotImplemented);

    _InputBoxes[0] = ui->headEdit ;
    _InputBoxes[1] = ui->bodyEdit ;
    _InputBoxes[2] = ui->armsEdit ;
    _InputBoxes[3] = ui->waistEdit;
    _InputBoxes[4] = ui->legsEdit ;

    this->_ArmorDataFound = _LoadConfigFiles();

    if (_Settings.find("Safe Mode") == _Settings.end())
        _Settings["Safe Mode"] = true;

    if (_Settings.find("Game Version") == _Settings.end())
        _Settings["Game Version"] = "Latest";
    
    if (_Settings.find("Steam Path") != _Settings.end())
    {
        std::string SteamPath = _Settings["Steam Path"].get<std::string>();
        _MHManager.setSteamDirectory(SteamPath);
        ui->actionSteam_Current->setText(("Current : " + SteamPath).c_str());
    }

    _PopulateVersionSelector();

    _PopulateComboBoxes();

    #ifndef NDEBUG
    QAction *tmp = new QAction(this);
    connect(tmp, QAction::triggered, this, _debugInputValue);
    tmp->setText("Debug Manual Input");
    ui->menuOptions->addAction(tmp); 
    #endif
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::show()
{
    if (!this->_ArmorDataFound)
    {
        DEBUG_LOG(ERROR, "Armor Data json file searched at " << ArmorDataFile<< " and not found");
        DialogWindow *Dia = new DialogWindow(nullptr, "ERROR FATAL", "Couldn't find " +
            ArmorDataFile.filename() +
            "\nIf It was deletd, re-download the program.", Status::ERROR0);
        Dia->getOkButton()->setStyleSheet("");
        Dia->show();
        this->close();
    }
    else if (!this->_MHManager.ProcessOpen())
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
            this->_UnsafeWarning();
            ui->actionSafe_Mode->setChecked(false);
        }
    }
}

void MainWindow::_show_test_gui() {QMainWindow::show();}

bool MainWindow::_LoadConfigFiles()
{
    if (SettingsFile.exists())
    {
        std::ifstream i(SettingsFile.str());
        i >> _Settings;
    }
    if (SavedSetsFile.exists())
    {
        std::ifstream i(SavedSetsFile.str());
        i >> _SavedSets;
    }
    if(!ArmorDataFile.exists())
        return false;
    
    std::ifstream i(ArmorDataFile.str());
    i >> _ArmorData;
    return true;
}

void MainWindow::_PopulateVersionSelector()
{
    QMenu *menuVersions = new QMenu(ui->menuOptions);
    menuVersions->setObjectName(QString::fromUtf8("menu_Versions"));
    menuVersions->setTitle("Select Game Version");
    ui->menuOptions->addAction(menuVersions->menuAction());

    _VersionGroup = new QActionGroup(this);

    QFont font;
    font.setBold(true);

    _VersionActions.reserve(MH_Memory::Versions.size());
    std::string TargetVersion = _Settings["Game Version"];
    for (auto it=MH_Memory::Versions.begin(); it!=MH_Memory::Versions.end(); ++it)
    {
        QAction *tmp = new QAction(this);
        connect(tmp, QAction::triggered, this, _UpdateSelectedVersion);
        tmp->setText(it->first.c_str());
        tmp->setCheckable(true);
        menuVersions->addAction(tmp);
        _VersionGroup->addAction(tmp);
        if (it->first == TargetVersion)
        {
            tmp->setChecked(true);
            tmp->setFont(font);
        }
        _VersionActions.push_back(tmp);
    }
    if (_VersionGroup->checkedAction() == 0)
    {
        DEBUG_LOG(ERROR, "Encountered unknown game version " << TargetVersion << " setting it to 'Latest'");
        _VersionActions.back()->setChecked(true);
        _VersionActions.back()->setFont(font);
    }
}

void MainWindow::_PopulateComboBoxes()
{
    int i;
    int ID;

    for (i = 0; i < 5; ++i)
        this->_InputBoxes[i]->addItem("Nothing", 255);

    for (auto &el : _ArmorData.items())
    {
        ID = el.value()["ID"];
        
        if (el.value()["Danger"])
        {
            _UnSafeArmors.insert(el.key());

            if (_Settings["Safe Mode"])
                continue;
        }

        for(i=0; i<5; ++i)
        {
            if (el.value()[Armor::Names[i]])
                this->_InputBoxes[i]->addItem(el.key().c_str(), ID);
        }
    }
    for (i = 0; i < 5; ++i)
        _SafeCount[i] = this->_InputBoxes[i]->count();
}