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
    ui->savedComboBox->addItems(_savedSets.keys());
}

void MainWindow::_loadSavedSet()
{
    if (ui->savedComboBox->currentIndex() == -1)
    {
        DialogWindow *Dia = new DialogWindow(this, "Warning", "No saved set selected", Status::WARNING);
        Dia->show();
        return;
    }
    auto currentText = ui->savedComboBox->currentText();

    for (int i = 0; i < 5; ++i)
        _MHManager.getPlayerData()[i] = _savedSets[currentText].toList()[i].toUInt();
    
    this->_updateArmorValues();
}

void MainWindow::_loadSavedSetPopup()
{
    if (_savedSets.empty())
    {
        DialogWindow *Dia = new DialogWindow(this, "Warning", "There are no saved sets", Status::WARNING);
        Dia->show();
        return;
    }
    
    bool ok;
    QString currentText = getItemInputDialog(this, "Select Saved Set", "Select set: ",
                                            _savedSets.keys(), &ok);
    if (!ok)
        return;
    int newIndex = std::max(0, ui->savedComboBox->findText(currentText));
    ui->savedComboBox->setCurrentIndex(newIndex);

    for (int i = 0; i < 5; ++i)
        _MHManager.getPlayerData()[i] = _savedSets[currentText].toList()[i].toUInt();
    this->_updateArmorValues();
}

void MainWindow::_saveCurrentSet()
{
    if (!this->_parseInputBoxes())
        return;

    auto currentText = ui->savedComboBox->currentText();
    if (_savedSets.find(currentText) == _savedSets.end())
        ui->savedComboBox->insertItem(0, currentText);
    
    auto data = _MHManager.getPlayerData();
    QVariantList dataVariant;
    for(const auto &v : data)
        dataVariant << QVariant(v);
    _savedSets[currentText] = dataVariant;

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
    auto currentText = ui->savedComboBox->currentText();
    ui->savedComboBox->removeItem(currentIdx);
    if (_savedSets.find(currentText) == _savedSets.end())
        return;
    _savedSets.erase(_savedSets.find(currentText));

    if (!this->_flushSavedSets())
    {
        DialogWindow *Dia = new DialogWindow(this, "ERROR", "Couldn't save to file.", Status::ERROR0);
        Dia->show();
        return;
    }
}

bool MainWindow::_flushSavedSets()
{
    if (!savedSetsFile.open(QIODevice::WriteOnly))
    {
        LOG_ENTRY(ERROR, "Couldn't open " << savedSetsFile);
        savedSetsFile.close();
        return false;
    }

    _savedSetsDocument.setObject(QJsonObject::fromVariantMap(_savedSets));
    auto data = _savedSetsDocument.toJson(QJsonDocument::Indented);
    if (savedSetsFile.write(data) == -1)
    {
        savedSetsFile.close();
        return false;
    }
    savedSetsFile.close();
    return true;
}
