#include "MainWindow.hpp"
#include "Config.h"
#include <QThread>
#include <QMovie>
#include <QInputDialog>
#include <QFileDialog>
#include <QActionGroup>

/// These file contains Member definitions of the MainWindow class
/// Related to initiliaze things before GUI is showed.

MainWindow::MainWindow(QString github_repo, QString current_version, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _settings(settingsFile.fileName(), QSettings::IniFormat, this),
    _updater(this, github_repo, current_version)
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
    connect(ui->actionNo_Backup_Ok, QAction::triggered, this, _toggleNoBackup);
    connect(ui->actionExit, QAction::triggered, this, close);

    connect(ui->SearchButton, QPushButton::released, this, _findAddress);
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
    connect(ui->actionAutoSteam, QAction::triggered, this, _setAutoSteam);
    connect(ui->actionAutomatically_Check_for_Updates, QAction::triggered, this, _toggleAutoUpdates);
    connect(ui->actionCheck_for_Updates, QAction::triggered, this, _checkForUpdates);

    _logGroup = new QActionGroup(this);
    _logGroup->addAction(ui->actionLevelError);
    _logGroup->addAction(ui->actionLevelWarning);
    _logGroup->addAction(ui->actionLevelDebug);
    for (auto act : _logGroup->actions())
        connect(act, QAction::triggered, this, _updateSelectedLogLevel);

    _inputBoxes[0] = ui->headEdit ;
    _inputBoxes[1] = ui->bodyEdit ;
    _inputBoxes[2] = ui->armsEdit ;
    _inputBoxes[3] = ui->waistEdit;
    _inputBoxes[4] = ui->legsEdit ;

    _loadConfigFiles();

    if (!_settings.value("SteamPath/Auto", true).toBool())
    {
        if (!_settings.value("SteamPath/CustomPath", "").toString().isEmpty())
        {
            auto steamPath = _settings.value("SteamPath/CustomPath").toString();
            _MHManager.setSteamDirectory(steamPath);
            ui->actionSteam_Current->setText(steamPath.prepend("Current : "));
            ui->actionAutoSteam->setChecked(false);
        }
        else
            _settings.setValue("SteamPath/Auto", true);
    }

    auto logLevel = _settings.value("Debug/LogLevel", "DEBUG").toString();
    if (LOG_HIERARCHY.find(logLevel) != LOG_HIERARCHY.end())
        _settings.setValue("Debug/LogLevel", "DEBUG");
    
    for (auto act : _logGroup->actions())
        if (act->text() == _settings.value("Debug/LogLevel").toString())
            act->setChecked(true);

    ui->actionNo_Backup_Ok->setChecked(_settings.value("General/NoBackupOk", false).toBool());

    _populateVersionSelector();
    _populateSavedSets();
    _populateLanguages();
    _translateArmorData();
    _updateSelectedLogLevel();

    if (_settings.value("General/AutoUpdates", true).toBool())
        _updater.checkForUpdates(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::_loadConfigFiles()
{
    if (savedSetsFile.exists())
    {
        std::ifstream i(savedSetsFile.fileName().toStdString());
        i >> _SavedSets;
    }

    armorDataFile.open(QIODevice::ReadOnly);
    _ArmorData = json::parse(armorDataFile.readAll());
    armorDataFile.close();
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

    _versionActions.reserve(MH_Memory::versions.size());
    auto targetVersion = _settings.value("General/GameVersion", "Latest").toString();
    for (auto it=MH_Memory::versions.begin(); it!=MH_Memory::versions.end(); ++it)
    {
        QAction *tmp = new QAction(this);
        connect(tmp, QAction::triggered, this, _updateSelectedVersion);
        tmp->setText(it->first);
        tmp->setCheckable(true);
        menuVersions->addAction(tmp);
        _versionGroup->addAction(tmp);
        if (it->first == targetVersion)
        {
            tmp->setChecked(true);
            tmp->setFont(font);
        }
        _versionActions.push_back(tmp);
    }
    if (_versionGroup->checkedAction() == 0)
    {
        LOG_ENTRY(ERROR, "Encountered unknown game version " << targetVersion << " setting it to 'Latest'");
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

    std::vector<QString> Langs;
    for (const auto &el :_ArmorData.front().items())
        if (el.key() != "Mode" && el.key() != "Danger")
            Langs.push_back(QString::fromStdString(el.key()));

    _langActions.reserve(Langs.size());
    auto targetLang = _settings.value("General/Language", "English").toString();
    for (const auto &la : Langs)
    {
        QAction *tmp = new QAction(this);
        connect(tmp, QAction::triggered, this, _translateArmorData);
        tmp->setText(la);
        tmp->setCheckable(true);
        menuLangs->addAction(tmp);
        _langGroup->addAction(tmp);
        if (la == targetLang)
        {
            tmp->setChecked(true);
            tmp->setFont(font);
        }
        _langActions.push_back(tmp);
    }
    if (_langGroup->checkedAction() == 0)
    {
        LOG_ENTRY(ERROR, "Encountered unknown language " << targetLang << " setting it to 'English'");
        _langActions.back()->setChecked(true);
        _langActions.back()->setFont(font);
    }
}

void MainWindow::_populateComboBoxes()
{
    int ID;
    std::string Mode;

    for (int i = 0; i < 5; ++i)
        this->_inputBoxes[i]->addItem("Nothing", Armor::NOTHING);

    for (auto &el : _transArmorData.items())
    {
        ID = el.value()["ID"];

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