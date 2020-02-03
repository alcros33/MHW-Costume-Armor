#include "MainWindow.hpp"
#include "Config.h"
#include <QThread>
#include <QMovie>
#include <QInputDialog>
#include <QFileDialog>
#include <QActionGroup>

/// These file contains Member definitions of the MainWindow class
/// Related to showing dialogs with information

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

void MainWindow::_notImplemented()
{
    DialogWindow *Dia = new DialogWindow(this, "Warning", "Functionality Not Implemented... Yet", Status::WARNING);
    Dia->show();
}

void MainWindow::_checkForUpdates()
{
    _updater.checkForUpdates(false);
}
