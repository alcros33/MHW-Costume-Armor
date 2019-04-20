#include "MainWindow.hpp"
#include "Config.h"
#include <QThread>
#include <QMovie>
#include <QInputDialog>
#include <QFileDialog>
#include <QActionGroup>

/// These file contains Member definitions of the MainWindow class
/// Related to showing dialogs with information

void MainWindow::_UnsafeWarning()
{
    if (_Settings.find("Disable Unsafe Warning") != _Settings.end())
        if (_Settings["Disable Unsafe Warning"] == true)
            return;

    this->setWindowTitle(QString(PROJECT_NAME) + "-UNSAFE");
    DEBUG_LOG(WARNING, "Safe Mode was turned off");
    DialogWindow *Dia = new DialogWindow(this, "Warning", "You are not Running in SafeMode\n(!) Marked Armors May Cause Game Crashes\nUse with caution.", Status::WARNING);
    Dia->show();
    this->_AddUnsafe();
}

void MainWindow::debugPrints() const
{
    if (_MHManager.SteamFound())
    {
        DEBUG_LOG(DEBUG, "Steam UserData ID: " << _MHManager.getSteamID());
        DEBUG_LOG(DEBUG, "Steam Game Directory: " << _MHManager.getSteamPath());
    }
    else
    {
        DEBUG_LOG(ERROR, "Couldn't Find Steam Data");
    }
}

void MainWindow::_aboutInfo()
{
    AboutWindow *Dia = new AboutWindow(this);
    Dia->show();
}

void MainWindow::_Instructions()
{
    Instructions *Dia = new Instructions(this);
    Dia->show();
}

void MainWindow::_NotImplemented()
{
    DialogWindow *Dia = new DialogWindow(this, "Warning", "Functionality Not Implemented... Yet", Status::WARNING);
    Dia->show();
}
