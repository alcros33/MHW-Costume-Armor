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
    std::ofstream Out(settingsFile.str());
    if (!Out)
    {
        DEBUG_LOG(ERROR, "Couldn't open " << savedSetsFile);
        Out.close();
        return false;
    }
    Out << std::setw(2) << _Settings << std::endl;
    Out.close();
    return true;
}

void MainWindow::_getCustomSteamPath()
{
    QString Dir = QFileDialog::getExistingDirectory(this, "Open Folder Containing Steam.exe", "C:\\", QFileDialog::ShowDirsOnly);
    if (Dir.isEmpty())
        return;
    _Settings["Steam Path"] = Dir.toStdString();
    this->_MHManager.setSteamDirectory(Dir.toStdString());
    ui->actionSteam_Current->setText(("Current : " + Dir.toStdString()).c_str());
    this->_flushSettings();
}
