#include "MainWindow.hpp"
#include "Config.h"
#include <QThread>
#include <QMovie>
#include <QInputDialog>
#include <QFileDialog>
#include <QActionGroup>

/// These file contains Member definitions of the MainWindow class
/// Related to the saving, loading and management of saved Armor Sets

void MainWindow::_LoadSavedSet()
{
    if (_SavedSets.empty())
    {
        DialogWindow *Dia = new DialogWindow(this, "Warning", "There are no saved sets", Status::WARNING);
        Dia->show();
        return;
    }

    QStringList items;
    for (const auto &it : _SavedSets.items())
        items << it.key().c_str();

    bool ok;
    QString text = getItemInputDialog(this, "Select Armor Set", "Select set: ", items, &ok);
    if (!ok)
        return;

    try
    {
        for (int i = 0; i < 5; ++i)
            _MHManager.getPlayerData().setArmorPiece(i, _SavedSets[text.toStdString()][i]);
    }
    catch (std::exception &e)
    {
        DialogWindow *Dia = new DialogWindow(this, "ERROR", "Invalid Value for armor", Status::ERROR0);
        Dia->show();
        DEBUG_LOG(ERROR, "Invalid value at json file. Error " << e.what());
        return;
    }
    this->_UpdateArmorValues();
}

void MainWindow::_SaveCurrentSet()
{
    if (!this->_ParseInputBoxes())
        return;

    bool ok;
    QString text;
    while (true)
    {
        text = getTextInputDialog(this, "Saving Armor set", "Name for the set", &ok);
        if (!ok)
            return;

        if (_SavedSets.find(text.toStdString()) != _SavedSets.end())
        {
            DialogWindow *Dia = new DialogWindow(this, "WARNING", "The name already exists.", Status::WARNING);
            Dia->show();
            while (Dia->isVisible())
                QCoreApplication::processEvents();
            continue;
        }

        if (!text.isEmpty())
            break;
    }

    auto Data = _MHManager.getPlayerData().getData();
    _SavedSets[text.toStdString()] = Data;

    if (!this->_FlushSavedSets())
    {
        DialogWindow *Dia = new DialogWindow(this, "ERROR", "Couldn't save set.", Status::ERROR0);
        Dia->show();
        return;
    }
    DialogWindow *Dia = new DialogWindow(this, "Success", "Successfully saved set!", Status::SUCCESS);
    Dia->show();
}

bool MainWindow::_FlushSavedSets()
{
    std::ofstream Out(SavedSetsFile.string());
    if (!Out)
    {
        DEBUG_LOG(ERROR, "Couldn't open " << SavedSetsFile);
        Out.close();
        return false;
    }
    Out << std::setw(2) << _SavedSets << std::endl;
    Out.close();
    return true;
}
