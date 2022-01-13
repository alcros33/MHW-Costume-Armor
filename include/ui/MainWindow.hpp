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
#include "ArmorData.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QString github_repo, QString current_version, QSettings &settings, QWidget *parent=0);
    ~MainWindow();
    QFile savedSetsFile = QCoreApplication::applicationDirPath() + "\\SavedSets.json";
    inline static constexpr float sizeRatio{1.6};

private:
    Ui::MainWindow *ui;
    QActionGroup *_versionGroup = nullptr;
    QActionGroup *_langGroup = nullptr;
    QActionGroup *_logGroup = nullptr;
    MH_Memory _MHManager;
    QSettings &_settings;
    QJsonDocument _savedSetsDocument;
    QVariantMap _savedSets;
    std::array<QComboBox*,5> _inputBoxes{nullptr};
    UpdaterGithub _updater;
    QMap<QWidget *, QPair<QSize, QPoint>> _baseGeo;
    void _setFontSize();
    QWidgetList _childCache;
    uint _qssLen;


protected:
    bool _ensureSizeRatio(QResizeEvent *event);
    void _resizeWidgets();
    void resizeEvent(QResizeEvent *event) final;

private slots:
    // Inits
    void _loadJsonFiles();
    void _initVersionSelector();
    void _initLanguages();
    // Dialogs
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
    void _fontScale();
    void _toggleLRArmors();
    //Saved Sets
    bool _flushSavedSets();
    void _saveCurrentSet();
    void _deleteCurrentSet();
    void _loadSavedSet();
    void _loadSavedSetPopup();
    //Armor
    void _translateArmorData();
    bool _parseInputBoxes();
    void _updateArmorValues();
    void _clearArmor();
    void _changeAll();
    void _manualInputValue();
};