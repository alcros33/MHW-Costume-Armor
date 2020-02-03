#include "MainWindow.hpp"
#include "Config.h"
#include <QThread>
#include <QMovie>
#include <QInputDialog>
#include <QFileDialog>
#include <QActionGroup>

/// These file contains Member definitions of the MainWindow class
/// Related to the saving, loading and management of saved Armor Sets
/// Also restoring backup

void MainWindow::_populateSavedSets()
{
    QStringList items;
    for (const auto &it : _SavedSets.items())
        items << it.key().c_str();
    
    ui->savedComboBox->addItems(items);
}

void MainWindow::_loadSavedSet()
{
    int currentIdx = ui->savedComboBox->currentIndex();

    if (currentIdx == -1)
    {
        DialogWindow *Dia = new DialogWindow(this, "Warning", "No saved set selected", Status::WARNING);
        Dia->show();
        return;
    }
    std::string currentText = ui->savedComboBox->currentText().toStdString();
    try
    {
        for (int i = 0; i < 5; ++i)
            _MHManager.getPlayerData()[i] = _SavedSets[currentText][i];
    }
    catch (std::exception &e)
    {
        DialogWindow *Dia = new DialogWindow(this, "ERROR", "Invalid Value for armor", Status::ERROR0);
        Dia->show();
        LOG_ENTRY(ERROR, "Invalid value at saved sets json file. Error " << e.what());
        return;
    }
    this->_updateArmorValues();
}

void MainWindow::_loadSavedSetPopup()
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
    QString text = getItemInputDialog(this, "Select Saved Set", "Select set: ", items, &ok);
    if (!ok)
        return;
    int newIndex = std::max(0, ui->savedComboBox->findText(text));
    ui->savedComboBox->setCurrentIndex(newIndex);
    try
    {
        for (int i = 0; i < 5; ++i)
            _MHManager.getPlayerData()[i] = _SavedSets[text.toStdString()][i];
    }
    catch (std::exception &e)
    {
        DialogWindow *Dia = new DialogWindow(this, "ERROR", "Invalid Value for armor", Status::ERROR0);
        Dia->show();
        LOG_ENTRY(ERROR, "Invalid value at saved sets json file. Error " << e.what());
        return;
    }
    this->_updateArmorValues();
}

void MainWindow::_saveCurrentSet()
{
    if (!this->_parseInputBoxes())
        return;

    std::string currentText = ui->savedComboBox->currentText().toStdString();
    if (_SavedSets.find(currentText) == _SavedSets.end())
        ui->savedComboBox->insertItem(0, currentText.data());

    auto Data = _MHManager.getPlayerData();
    _SavedSets[currentText] = Data;

    if (!this->_flushSavedSets())
    {
        DialogWindow *Dia = new DialogWindow(this, "ERROR", "Couldn't save to file.", Status::ERROR0);
        Dia->show();
        return;
    }
    DialogWindow *Dia = new DialogWindow(this, "Success", "Successfully saved set!", Status::SUCCESS);
    Dia->show();
}

void MainWindow::_deleteCurrentSet()
{
    int currentIdx = ui->savedComboBox->currentIndex();

    if (currentIdx == -1)
    {
        DialogWindow *Dia = new DialogWindow(this, "Warning", "No saved set selected", Status::WARNING);
        Dia->show();
        return;
    }
    std::string currentText = ui->savedComboBox->currentText().toStdString();
    ui->savedComboBox->removeItem(currentIdx);
    if (_SavedSets.find(currentText) == _SavedSets.end())
        return;
    _SavedSets.erase(currentText);
    
    if (!this->_flushSavedSets())
    {
        DialogWindow *Dia = new DialogWindow(this, "ERROR", "Couldn't save to file.", Status::ERROR0);
        Dia->show();
        return;
    }
}

bool MainWindow::_flushSavedSets()
{
    std::ofstream Out(savedSetsFile.fileName().toStdString());
    if (!Out)
    {
        LOG_ENTRY(ERROR, "Couldn't open " << savedSetsFile);
        Out.close();
        return false;
    }
    Out << std::setw(2) << _SavedSets << std::endl;
    Out.close();
    return true;
}
