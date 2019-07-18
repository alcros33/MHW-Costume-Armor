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

void MainWindow::_findAddr()
{
    ui->SearchButton->setText("Searching for Save Data...");
    ui->SearchButton->setEnabled(false);

    DialogWindow *Dia = new DialogWindow(this, "Wait a Sec...", "Searching MHW for Character Data", Status::SUCCESS);
    Dia->setWindowFlags(Qt::SplashScreen);
    QLabel *IconLabel = Dia->getIconLabel();
    QString PrevText = IconLabel->text(); // Save it for later
    QMovie *movie = new QMovie(":/ajax-loader.gif");
    movie->setScaledSize(IconLabel->frameSize());
    IconLabel->setMovie(movie);
    movie->start();

    Dia->getOkButton()->setText("Cancel");
    Dia->setAttribute(Qt::WA_DeleteOnClose, false);

    Dia->show();

    std::string errorMessage = "";

    QThread *thread = QThread::create([this, &errorMessage] {
        try
        {
            this->_MHManager.findAddress(this->_Settings["Game Version"]);
        }
        catch (const std::exception &ex)
        {
            errorMessage = ex.what();
        }
    });

    thread->start();
    while (thread->isRunning())
    {
        if (!Dia->isVisible()) // User presed "Cancel"
        {
            thread->quit();
            delete Dia;
            ui->SearchButton->setEnabled(true);
            ui->SearchButton->setText("Search For MHW Save Data");
            return;
        }
        QCoreApplication::processEvents();
    }
    if (!errorMessage.empty())
    // If errorMessage is NOT empty, that means VritualQueryEx returned error code
    {
        Dia->setAttribute(Qt::WA_DeleteOnClose, true);
        Dia->close();
        ui->SearchButton->setText("Search For MHW Save Data");
        ui->SearchButton->setEnabled(true);
        DEBUG_LOG(ERROR, errorMessage);
        DialogWindow *Dia = new DialogWindow(this, "ERROR",
                                             "Error occurred during the searching process.\n"
                                             "Check that the program is updated\n"
                                             "Consider running the program as Adminstrator.", Status::ERROR0);
        Dia->show();
        return;
    }

    if (!this->_MHManager.dataAddressFound())
    {
        Dia->setAttribute(Qt::WA_DeleteOnClose, true);
        Dia->close();
        ui->SearchButton->setEnabled(true);
        DialogWindow *Dia = new DialogWindow(this, "ERROR", "Couldn't Find Save Data Address", Status::ERROR0);
        Dia->show();
        ui->SearchButton->setText("Search For MHW Save Data");
        return;
    }
    Dia->getOkButton()->setText("Ok");
    Dia->getIconLabel()->clear();
    Dia->getIconLabel()->setText(PrevText);
    Dia->getMsgLabel()->setText("MHW Data Found Successfully!");
    Dia->setWindowTitle("Succes!!");
    Dia->setAttribute(Qt::WA_DeleteOnClose, true);

    ui->SearchButton->setText("Ready To Go.");
    ui->SearchButton->setEnabled(false);

    ui->FetchButton->setEnabled(true);
    ui->WriteButton->setEnabled(true);
}
void MainWindow::_fetchData(bool noMessage)
{
    int slot = std::stoi(ui->comboBox->currentText().toStdString());
    if (!_MHManager.fetchPlayerData(slot - 1))
    {
        DialogWindow *Dia = new DialogWindow(this, "ERROR", "Couldn't Fetch Character Data...", Status::ERROR0);
        Dia->show();
        return;
    }
    this->_updateArmorValues();
    std::string msg = "Successfully fetched Data for Character Slot " + std::to_string(slot);
    if (!noMessage)
    {
        DialogWindow *Dia = new DialogWindow(this, "Success!!", msg, Status::SUCCESS);
        Dia->show();
    }
}

void MainWindow::_writeData()
{
    if (!this->_parseInputBoxes())
        return;

    int slot = std::stoi(ui->comboBox->currentText().toStdString());
    if (!_MHManager.writeArmor(slot - 1, _Settings["Safe Mode"]))
    {
        DialogWindow *Dia = new DialogWindow(this, "ERROR", "Couldn't Write Save Data!", Status::ERROR0);
        Dia->show();
    }
    else
    {
        DialogWindow *Dia = new DialogWindow(this, "Success!!", "Success writing to Game!!"
                                                                "\nYou may preview the appearance by checking your Guild Card"
                                                                "\nBut be sure to reload by entering another area.",
                                             Status::SUCCESS);
        Dia->show();
    }
    this->_fetchData(true);
}