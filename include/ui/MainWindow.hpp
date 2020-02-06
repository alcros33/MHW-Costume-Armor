#pragma once
#include <set>
#include <iomanip>
#include <map>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMainWindow>
#include <QSettings>
#include "ui_MainWindow.h"
#include "AboutWindow.hpp"
#include "DialogWindow.hpp"
#include "UpdaterGithub.hpp"
#include "MHMemory.hpp"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QString github_repo, QString current_version, QWidget *parent=0);
    ~MainWindow();

    QFile settingsFile = QCoreApplication::applicationDirPath() + "\\settings.ini";
    QFile savedSetsFile = QCoreApplication::applicationDirPath() + "\\SavedSets.json";
    QFile armorDataFile = QFile(":/ArmorData.json");

private:
    Ui::MainWindow *ui;
    QActionGroup *_versionGroup = nullptr;
    QActionGroup *_langGroup = nullptr;
    QActionGroup *_logGroup = nullptr;
    MH_Memory _MHManager;
    QSettings _settings;
    QJsonDocument _savedSetsDocument;
    QVariantMap _savedSets;
    QVariantMap _armorData;
    QVariantMap _transArmorData;
    std::array<QComboBox*,5> _inputBoxes{nullptr};
    UpdaterGithub _updater;

private slots:
    // Inits
    void _loadJsonFiles();
    void _populateComboBoxes();
    void _populateVersionSelector();
    void _populateLanguages();
    // Dialogs
    void _instructions();
    void _showLog();
    void _checkForUpdates();
    void _aboutInfo();
    void _notImplemented();
    // Memory
    void _setupForSearch(bool silent);
    void _findAddress();
    void _writeData();
    void _fetchData(bool noMessage = false);
    //Settings
    void _updateSelectedLogLevel();
    void _updateSelectedVersion();
    void _getCustomSteamPath();
    void _toggleNoBackup();
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
};