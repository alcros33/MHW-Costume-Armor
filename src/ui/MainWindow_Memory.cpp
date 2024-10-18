#include "MainWindow.hpp"
#include "Config.h"
#include <QThread>
#include <QMovie>
#include <QInputDialog>
#include <QFileDialog>
#include <QActionGroup>
#include <QMessageBox>

/// These file contains Member definitions of the MainWindow class
/// Related to readying and writting to memory.

void MainWindow::_findAddress()
{
    if (!this->_MHManager.openProcess())
    {
        DialogWindow *Dia = new DialogWindow(this, "ERROR", "MHW World doesn't seem to be open",
                                             Status::ERROR0);
        Dia->show();
        return;
    }

    ui->SearchButton->setText("Searching for Save Data...");
    ui->SearchButton->setEnabled(false);

    DialogWindow *Dia = new DialogWindow(this, "Wait a Sec...",
                                        "Searching MHW for Character Data", Status::SUCCESS);

    QString prevText = Dia->getIconLabel()->text(); // Save it for later
    Dia->getOkButton()->setText("Cancel");
    Dia->setAttribute(Qt::WA_DeleteOnClose, false);
    Dia->setWindowFlags(Qt::SplashScreen);

    QMovie *movie = new QMovie(":/ajax-loader.gif");
    movie->setScaledSize(Dia->getIconLabel()->frameSize());
    Dia->getIconLabel()->setMovie(movie);
    movie->start();

    Dia->show();

    QThread *thread = QThread::create([this] {
            if (!this->_MHManager.steamFound())
                this->_MHManager.findSteamPath();
            this->_MHManager.findDataAddress(_settings.value("General/GameVersion", "Latest").toString());
    });

    thread->start();
    while (thread->isRunning())
    {
        if (!Dia->isVisible()) // User presed "Cancel"
        {
            thread->quit();
            Dia->setAttribute(Qt::WA_DeleteOnClose, true);
            Dia->close();
            this->_setupForSearch(true);
            return;
        }
        QCoreApplication::processEvents();
    }

    if (!this->_MHManager.dataAddressFound())
    {
        Dia->setAttribute(Qt::WA_DeleteOnClose, true);
        Dia->close();
        this->_setupForSearch(true);
        DialogWindow *Dia = new DialogWindow(this, "ERROR",
                                             "Error occurred during the searching process.\n"
                                             "Check that the program is updated\n"
                                             "Consider running the program as Adminstrator.", Status::ERROR0);
        Dia->show();
        return;
    }

    Dia->getOkButton()->setText("Ok");
    Dia->getIconLabel()->clear();
    Dia->getIconLabel()->setText(prevText);
    Dia->getMsgLabel()->setText("MHW Character Data Found Successfully!");
    Dia->setWindowTitle("Succes!!");

    Dia->setAttribute(Qt::WA_DeleteOnClose, true);
    ui->SearchButton->setText("Ready To Go.");
    ui->SearchButton->setEnabled(false);

    ui->FetchButton->setEnabled(true);
    ui->WriteButton->setEnabled(true);
}

void MainWindow::_fetchData(bool noMessage)
{
    if (!this->_MHManager.validateProcess())
    {
        this->_setupForSearch(false);
        return;
    }
    
    int slot = ui->comboBox->currentText().toInt();
    if (!_MHManager.readArmor(slot - 1))
    {
        DialogWindow *Dia = new DialogWindow(this, "ERROR", "Couldn't Fetch Character Data...", Status::ERROR0);
        Dia->show();
        return;
    }
    this->_updateArmorValues();
    QString msg = "Successfully fetched Data for Character Slot %1";
    if (!noMessage)
    {
        DialogWindow *Dia = new DialogWindow(this, "Success!!", msg.arg(slot), Status::SUCCESS);
        Dia->show();
    }
}

void MainWindow::_writeData()
{
    if (!this->_MHManager.validateProcess())
    {
        this->_setupForSearch(false);
        return;
    }

    if (!this->_parseInputBoxes())
        return;

    uint slot = ui->comboBox->currentText().toUInt();
    if (!_MHManager.writeArmor(slot - 1, _settings.value("General/NoBackupOk", true).toBool()))
    {
        DialogWindow *Dia = new DialogWindow(this, "ERROR", "Couldn't Write Save Data!", Status::ERROR0);
        Dia->show();
    }
    else
    {
        DialogWindow *Dia = new DialogWindow(this, "Success!!", "Success writing to Game!!"
                                                                "\nYou may preview the appearance by checking your Guild Card"
                                                                "\nBut be sure to reload by entering another area before saving the game.",
                                             Status::SUCCESS);
        Dia->show();
    }
    this->_fetchData(true);
}

void MainWindow::_setupForSearch(bool silent)
{
    ui->SearchButton->setText("Search MHW Character Data");
    ui->SearchButton->setEnabled(true);
    ui->FetchButton->setEnabled(false);
    ui->WriteButton->setEnabled(false);
    if (silent)
        return;
    
}