#include <QThread>
#include <QMovie>
#include <QInputDialog>
#include <QFileDialog>
#include <QActionGroup>
#include "MainWindow.hpp"
#include "Config.h"

/// These file contains Member definitions of the MainWindow class
/// Related to showing dialogs with information

void MainWindow::_aboutInfo()
{
    AboutWindow *Dia = new AboutWindow(this, _settings.value("General/FontBaseSize", 8.F).toFloat());
    Dia->show();
}

void MainWindow::_checkForUpdates()
{
    _updater.checkForUpdates(false, _settings.value("General/FontBaseSize", 1.0F).toFloat());
}

void MainWindow::_notImplemented()
{
    DialogWindow *Dia = new DialogWindow(this, "Warning", "Functionality Not Implemented... Yet", Status::WARNING);
    Dia->show();
}