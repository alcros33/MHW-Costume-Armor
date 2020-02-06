#include <QThread>
#include <QMovie>
#include <QInputDialog>
#include <QFileDialog>
#include <QActionGroup>
#include "MainWindow.hpp"
#include "Config.h"
#include "LogViewer.hpp"

/// These file contains Member definitions of the MainWindow class
/// Related to showing dialogs with information

void MainWindow::_showLog()
{
    auto dia = new LogWindow(this, _MHManager.exeDir.absoluteFilePath("CostumeArmor.log"),
                            {"Date", "Time", "Level", "Message"}, {2});
    dia->show();
}

void MainWindow::_aboutInfo()
{
    AboutWindow *Dia = new AboutWindow(this);
    Dia->show();
}

void MainWindow::_instructions()
{
    Instructions *Dia = new Instructions(this);
    Dia->show();
}

void MainWindow::_checkForUpdates()
{
    _updater.checkForUpdates(false);
}

void MainWindow::_notImplemented()
{
    DialogWindow *Dia = new DialogWindow(this, "Warning", "Functionality Not Implemented... Yet", Status::WARNING);
    Dia->show();
}