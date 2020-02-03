#pragma once
#include <QMainWindow>
#include <set>
#include <iomanip>
#include <map>

#include "ui_MainWindow.h"
#include "AboutWindow.hpp"
#include "DialogWindow.hpp"
#include "UpdaterGithub.hpp"

#include "MHMemory.hpp"
#include "json.hpp"

// for convenience
using json = nlohmann::json;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QString github_repo, QString current_version, QWidget *parent=0);
    ~MainWindow();

    void show();

    QFile settingsFile = get_appdata_dir().absoluteFilePath("Settings.json");
    QFile savedSetsFile = get_appdata_dir().absoluteFilePath("SavedSets.json");
    QFile armorDataFile = QFile(":/ArmorData.json");

private:
    Ui::MainWindow *ui;
    QActionGroup *_versionGroup = nullptr;
    QActionGroup *_langGroup = nullptr;
    QActionGroup *_logGroup = nullptr;
    MH_Memory _MHManager;
    json _Settings;
    json _SavedSets;
    json _ArmorData;
    std::array<QComboBox*,5> _inputBoxes = {nullptr,nullptr,nullptr,nullptr,nullptr};
    std::vector<QAction*> _versionActions;
    std::vector<QAction*> _langActions;
    std::set<std::string> _unsafeArmors;
    std::array<int,5> _safeCount = {0,0,0,0,0};
    json _transArmorData;
    UpdaterGithub _updater;

private slots:
    // Inits
    void _loadConfigFiles();
    void _populateComboBoxes();
    void _populateVersionSelector();
    void _populateLanguages();
    void _setDefaultSettings();
    void _updateSelectedLogLevel();
    // Dialogs
    void _instructions();
    void _checkForUpdates();
    void _aboutInfo();
    void _unsafeWarning();
    void _notImplemented();
    // Memory
    void _setupForSearch(bool silent);
    void _findAddress();
    void _writeData();
    void _fetchData(bool noMessage = false);
    //Settings
    void _updateSelectedVersion();
    bool _flushSettings();
    void _getCustomSteamPath();
    void _toggleSafe();
    void _setAutoSteam();
    void _toggleAutoUpdates();
    //Saved Sets
    bool _flushSavedSets();
    void _saveCurrentSet();
    void _deleteCurrentSet();
    void _loadSavedSet();
    void _loadSavedSetPopup();
    void _populateSavedSets();
    //Armor
    void _translateArmorData();
    bool _parseInputBoxes();
    void _updateArmorValues();
    void _clearArmor();
    void _changeAll();
    void _manualInputValue();
    void _addUnsafe();
    void _deleteUnsafe();
};