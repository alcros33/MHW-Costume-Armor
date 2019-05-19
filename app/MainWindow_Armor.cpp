#include "MainWindow.hpp"
#include "Config.h"
#include <QThread>
#include <QMovie>
#include <QInputDialog>
#include <QFileDialog>
#include <QActionGroup>
#include <QMessageBox>

/// These file contains Member definitions of the MainWindow class
/// Related to The managment of input Comboboxes

void MainWindow::_updateArmorValues()
{
    auto Data = _MHManager.getPlayerData().getData();
    int index;
    for (int i = 0; i < 5; ++i)
    {
        index = _inputBoxes[i]->findData((int)Data[i]);
        if (index < 0)
        {
            index = 0;
            DEBUG_LOG(WARNING, "Encountered unknown value (" << (int)Data[i] << ") changing it to 255 for safety");
        }
        _inputBoxes[i]->setCurrentIndex(index);
    }
}

void MainWindow::_clearArmor()
{
    for(int i=0; i<5 ;++i)
        _MHManager.getPlayerData().setArmorPiece(i, 255);
    this->_updateArmorValues();
}

bool MainWindow::_parseInputBoxes()
{
    int Val;
    for (int i = 0; i < 5; ++i)
    {
        Val = _inputBoxes[i]->currentData().toInt();
        if (Val == 0) // Means Error btw
        {
            DialogWindow *Dia = new DialogWindow(this, "ERROR", "Invalid Value for armor", Status::ERROR0);
            Dia->show();
            this->_fetchData(true);
            return false;
        }
        this->_MHManager.getPlayerData().setArmorPiece(i, Val);
    }
    return true;
}

void MainWindow::_addUnsafe()
{
    int i;
    int ID;
    for(const auto &_Name : this->_unsafeArmors)
    {
        ID = _ArmorData[_Name]["ID"];
        for (i = 0; i < 5; ++i)
        {
            if (_ArmorData[_Name][Armor::Names[i]])
                this->_inputBoxes[i]->addItem(std::string("(!) ").append(_Name).c_str(), ID);
        }
    }
}

void MainWindow::_deleteUnsafe()
{
    for (int i = 0; i < 5; ++i)
    {
        for (int index = _inputBoxes[i]->count(); index >= _safeCount[i]; --index)
            _inputBoxes[i]->removeItem(index);
    }
}

void MainWindow::_changeAll()
{
    QStringList items;
    bool isSet;
    int i = 0;
    for (auto &it : _ArmorData.items())
    {
        if (it.value()["Danger"] && _Settings["Safe Mode"])
            continue;

        isSet = true;
        for(i=0; i<5; ++i)
            if ( !it.value()[Armor::Names[i]] )
            {
                isSet = false;
                break;
            }
        
        if(isSet)
            items << it.key().c_str();
    }

    bool ok;
    QString text = getItemInputDialog(this, "Change All Armor", "Select set: ", items, &ok);
    if (!ok)
        return;
    json Selected = _ArmorData[text.toStdString()];
    for (i = 0; i < 5; ++i)
        _MHManager.getPlayerData().setArmorPiece(i, Selected["ID"] );
    
    this->_updateArmorValues();
}

void MainWindow::_debugInputValue()
{
    bool ok;
    int id = QInputDialog::getInt(this, "Manually Input Id",
                                 "Input Armor ID", 255, 1, 255, 1, &ok);
    if (!ok)
        return;
    for (int i = 0; i < 5; ++i)
        _MHManager.getPlayerData().setArmorPiece(i, id);
    int slot = std::stoi(ui->comboBox->currentText().toStdString());
    _MHManager.writeArmor(slot - 1, _Settings["Safe Mode"]);
}