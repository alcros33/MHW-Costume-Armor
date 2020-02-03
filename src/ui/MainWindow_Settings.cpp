#include "MainWindow.hpp"
#include "Config.h"
#include <QThread>
#include <QMovie>
#include <QInputDialog>
#include <QFileDialog>
#include <QActionGroup>

/// These file contains Member definitions of the MainWindow class
/// Related to handle settings

void MainWindow::_updateSelectedVersion()
{
    QAction *checked = _versionGroup->checkedAction();
    _Settings["Game Version"] = checked->text().toStdString();

    QFont font;
    for (auto verAction : _versionActions)
        verAction->setFont(font);

    font.setBold(true);
    checked->setFont(font);
    this->_flushSettings();
}

void MainWindow::_updateSelectedLogLevel()
{
    QAction *checked = _logGroup->checkedAction();
    _Settings["Log Level"] = checked->text().toStdString();
    GLOBAL_LOG_LEVEL = LOG_NAMES[_Settings["Log Level"]];

    QFont font;
    for (auto act : _logGroup->actions())
        act->setFont(font);

    font.setBold(true);
    checked->setFont(font);
    this->_flushSettings();
}

void MainWindow::_toggleSafe()
{
    _Settings["Safe Mode"] = !_Settings["Safe Mode"];
    if (!_Settings["Safe Mode"])
    {
        this->_unsafeWarning();
    }
    else
    {
        this->_deleteUnsafe();
        this->setWindowTitle(PROJECT_NAME);
    }
    _flushSettings();
}

bool MainWindow::_flushSettings()
{
    std::ofstream Out(settingsFile.fileName().toStdString());
    if (!Out)
    {
        LOG_ENTRY(ERROR, "Couldn't open " << settingsFile);
        Out.close();
        return false;
    }
    Out << std::setw(2) << _Settings << std::endl;
    Out.close();
    return true;
}

void MainWindow::_getCustomSteamPath()
{
    QString Dir = QFileDialog::getExistingDirectory(this, "Select the folder", "C:\\", QFileDialog::ShowDirsOnly);
    if (Dir.isEmpty())
        return;
    _Settings["Steam Path"] = Dir.toStdWString();
    _Settings["Auto Steam Path"] = false;
    ui->actionAutoSteam->setChecked(false);
    this->_MHManager.setSteamDirectory(Dir);
    ui->actionSteam_Current->setText(("Current : " + Dir.toStdString()).c_str());
    this->_flushSettings();
}

void MainWindow::_setAutoSteam()
{
    if (_Settings["Auto Steam Path"])
    {
        ui->actionAutoSteam->setChecked(true);
        return;
    }

    _Settings["Auto Steam Path"] = true;
    ui->actionAutoSteam->setChecked(true);
    this->_MHManager.unSetSteamDirectory();
    this->_flushSettings();
    if (!this->_MHManager.processIsOpen())
    {
        auto Dia = new DialogWindow(this, "Automatic Steam Path", "Steam path has been set to automatic, but MHW seems to be closed. Open it and press Search MHW Character Data.", Status::WARNING);
        Dia->show();
        return;
    }
    this->_MHManager.findSteamPath();
}

void MainWindow::_toggleAutoUpdates()
{
    _Settings["Auto Steam Path"] = !_Settings["Auto Steam Path"];
    this->_flushSettings();
}