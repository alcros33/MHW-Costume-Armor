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

void MainWindow::_UpdateArmorValues()
{
    auto Data = _MHManager.getPlayerData().getData();
    int index;
    for (int i = 0; i < 5; ++i)
    {
        index = _InputBoxes[i]->findData((int)Data[i]);
        if (index < 0)
        {
            index = 0;
            DEBUG_LOG(WARNING, "Encountered unknown value (" << (int)Data[i] << ") changing it to 255 for safety");
        }
        _InputBoxes[i]->setCurrentIndex(index);
    }
}

void MainWindow::_ClearArmor()
{
    for(int i=0; i<5 ;++i)
        _MHManager.getPlayerData().setArmorPiece(i, 255);
    this->_UpdateArmorValues();
}

bool MainWindow::_ParseInputBoxes()
{
    int Val;
    for (int i = 0; i < 5; ++i)
    {
        Val = _InputBoxes[i]->currentData().toInt();
        if (Val == 0) // Means Error btw
        {
            DialogWindow *Dia = new DialogWindow(this, "ERROR", "Invalid Value for armor", Status::ERROR0);
            Dia->show();
            this->_FetchData(true);
            return false;
        }
        this->_MHManager.getPlayerData().setArmorPiece(i, Val);
    }
    return true;
}

void MainWindow::_AddUnsafe()
{
    int i;
    int ID;
    for(const auto &_Name : this->_UnSafeArmors)
    {
        ID = _ArmorData[_Name]["ID"];
        for (i = 0; i < 5; ++i)
        {
            if (_ArmorData[_Name][Armor::Names[i]])
                this->_InputBoxes[i]->addItem(std::string("(!) ").append(_Name).c_str(), ID);
        }
    }
}

void MainWindow::_DeleteUnsafe()
{
    for (int i = 0; i < 5; ++i)
    {
        for (int index = _InputBoxes[i]->count(); index >= _SafeCount[i]; --index)
            _InputBoxes[i]->removeItem(index);
    }
}

void MainWindow::_ChangeAll()
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
    
    this->_UpdateArmorValues();
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
    _MHManager.WriteArmor(slot - 1, _Settings["Safe Mode"]);
}