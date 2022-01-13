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

void MainWindow::_translateArmorData()
{
    QAction *checked = _langGroup->checkedAction();
    _settings.setValue("General/Language", checked->text());
    auto Lang = checked->text();

    QFont font;
    for (auto lAction : _langGroup->actions())
        lAction->setFont(font);

    font.setBold(true);
    checked->setFont(font);
    for (int i = 0; i < 5; ++i)
        this->_inputBoxes[i]->clear();
    this->_settings.sync();

    bool lr = _settings.value("General/LRArmors", false).toBool();

    // translate comboboxes
    auto &tArmorData = armorData.at(_langGroup->checkedAction()->text());
    for (uint id = 0; id < armor_count(); id++)
    {
        if (id < 37 && !lr)
            continue;
        uint mode = armorMode.at(id);
        auto &name = tArmorData.at(id);
        for (int i = 0; i < 5; i++)
            if (mode & (1 << i))
                this->_inputBoxes[i]->addItem(name, id);
    }
    for (int i = 0; i < 5; i++)
    {
        this->_inputBoxes[i]->model()->sort(0);
        // todo translate "nothing" too
        this->_inputBoxes[i]->insertItem(0, "Nothing", Armor::NOTHING);
        this->_inputBoxes[i]->setCurrentIndex(0);
    }
}

void MainWindow::_updateArmorValues()
{
    auto Data = _MHManager.getPlayerData();
    int index;
    for (int i = 0; i < 5; ++i)
    {
        index = _inputBoxes[i]->findData(Data[i]);
        if (index < 0)
        {
            index = 0;
            LOG_ENTRY(WARNING, "Encountered unknown value (" << Data[i] << ") changing it to "
                     << Armor::NOTHING << " for safety");
        }
        _inputBoxes[i]->setCurrentIndex(index);
    }
}

void MainWindow::_clearArmor()
{
    for(int i=0; i<5 ;++i)
        _MHManager.getPlayerData()[i] = Armor::NOTHING;
    this->_updateArmorValues();
}

bool MainWindow::_parseInputBoxes()
{
    int Val;
    for (int i = 0; i < 5; ++i)
    {
        Val = _inputBoxes[i]->currentData().toUInt();
        if (Val == 0) // Means Error btw
        {
            DialogWindow *Dia = new DialogWindow(this, "ERROR", "Invalid Value for armor", Status::ERROR0);
            Dia->show();
            this->_fetchData(true);
            return false;
        }
        this->_MHManager.getPlayerData()[i] = Val;
    }
    return true;
}

void MainWindow::_changeAll()
{
    QStringList names;
    QList<uint> idx;
    uint i;
    bool lr = _settings.value("General/LRArmors", false).toBool();
    auto& tArmorData = armorData.at(_langGroup->checkedAction()->text());
    for (i = 0; i<armor_count(); i++)
        if (armorMode.at(i) == 31 && (i>37||lr)) // Means its a set
        {
            names << tArmorData.at(i);
            idx << i;
        }
    
    auto [aName, aId, ok] = getItemInputDialog(this, "Change All Armor", "Select set: ", names, idx);
    if (!ok)
        return;
    for (i = 0; i < 5; ++i)
        _MHManager.getPlayerData()[i] = aId;

    this->_updateArmorValues();
}

void MainWindow::_manualInputValue()
{
    bool ok;
    int id = QInputDialog::getInt(this, "Manually Input ID",
                                  "Input Armor ID", 1, 1, 2147483647, 1, &ok);
    if (!ok)
        return;
    static auto customValue = QStringLiteral("Custom ID : %1");
    auto cid = customValue.arg(id);
    for (int i = 0; i < 5; ++i)
    {
        this->_inputBoxes[i]->insertItem(0, cid, id);
        this->_inputBoxes[i]->setCurrentIndex(0);
    }
}

void MainWindow::_toggleLRArmors()
{
    _settings.setValue("General/LRArmors", !_settings.value("General/LRArmors", false).toBool());
    _settings.sync();
    _translateArmorData();
}