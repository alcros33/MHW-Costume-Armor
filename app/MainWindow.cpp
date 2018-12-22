#include "MainWindow.hpp"
#include "Config.h"

// TODO PROGRESS BAR WITH QMovie

/// Begin Main Window Member definitions

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _InputBoxes(5)
{
    ui->setupUi(this);
    this->setWindowTitle(PROJECT_NAME);
    ui->FetchButton->setEnabled(false);
    ui->WriteButton->setEnabled(false);

    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(_aboutInfo()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()) );

    connect(ui->SearchButton, SIGNAL(released()), this, SLOT(_FindAddr()));
    connect(ui->FetchButton, SIGNAL(released()), this, SLOT(_FetchData()));
    connect(ui->WriteButton, SIGNAL(released()), this, SLOT(_WriteData()));

    _InputBoxes[0] = ui->headLineEdit ;
    _InputBoxes[1] = ui->bodyLineEdit ;
    _InputBoxes[2] = ui->armsLineEdit ;
    _InputBoxes[3] = ui->waistLineEdit;
    _InputBoxes[4] = ui->legsLineEdit ;
}

void MainWindow::show()
{
    QMainWindow::show();
    if (!this->_MHManager.ProcessOpen())
    {
        DialogWindow *Dia = new DialogWindow(nullptr, "ERROR", "MHW is Closed\nPlease open it before starting.", Status::ERROR0);
        Dia->show();
        this->close();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::_aboutInfo()
{
    AboutWindow *Dia = new AboutWindow(this);
    Dia->show();
}

void MainWindow::_FindAddr()
{
    ui->SearchButton->setText("Searching for Save Data...");
    ui->SearchButton->setEnabled(false);

    this->_MHManager.FindAddress();
    if (!this->_MHManager.DataAddressFound())
    {
        ui->SearchButton->setEnabled(true);
        DialogWindow *Dia = new DialogWindow(this, "ERROR", "Couldn't Find Save Data Address", Status::ERROR0);
        Dia->show();
        ui->SearchButton->setText("Search For MHW Save Data");
        return;
    }

    ui->SearchButton->setText("Ready To Go.");
    ui->SearchButton->setEnabled(false);

    ui->FetchButton->setEnabled(true);
    ui->WriteButton->setEnabled(true);
}

void MainWindow::_FetchData(bool noMessage)
{
    int slot = std::stoi(ui->comboBox->currentText().toUtf8().constData());
    if (!_MHManager.FetchPlayerData(slot-1))
    {
        DialogWindow *Dia = new DialogWindow(this, "ERROR", "Couldn't Fetch Character Data...", Status::ERROR0);
        Dia->show();
        return;
    }
    auto Data = _MHManager.getPlayerData().getDataString();
    for(int i=0;i<5;++i)
        _InputBoxes[i]->setText(Data[i].c_str());

    std::string msg = "Sucessfully fetched Data for Character Slot " + std::to_string(slot);
    if(!noMessage)
    {
        try{
        DialogWindow *Dia = new DialogWindow(this, "Sucess!!", msg, Status::SUCCESS);
        Dia->show();
        }
        catch (std::exception &e)
        {
            DEBUG_LOG(e.what());
        }
    }
}

void MainWindow::_WriteData()
{
    int val;
    for (int i = 0; i < 5; ++i)
    {
        try
        {
            std::string strVal = _InputBoxes[i]->text().toUtf8().constData();
            
            if (strVal == "")
                val = 255;
            else
                val = std::stoi(strVal);
            this->_MHManager.getPlayerData().setArmorPiece(i,val);
        }
        catch (std::exception &e)
        {

            DialogWindow *Dia = new DialogWindow(this, "ERROR", "Invalid Value for armor", Status::ERROR0);
            Dia->show();
            this->_FetchData(true);
            return;
        }
    }

    int slot = std::stoi(ui->comboBox->currentText().toUtf8().constData());
    if (!_MHManager.WriteArmor(slot-1,_SafeMode))
    {
        DialogWindow *Dia = new DialogWindow(this, "ERROR", "Couldn't Write Save Data!", Status::ERROR0);
        Dia->show();
    }
    else
    {
        DialogWindow *Dia = new DialogWindow(this, "Sucess!!", "Sucess writting to Game!!\nEnter you room to reload (Do not save before reloading!)", Status::SUCCESS);
        Dia->show();
    }
    this->_FetchData(true);
}

void MainWindow::debugPrints() const
{
    if (_MHManager.SteamFound())
    {
        std::cout << "\tSteam UserData ID: " << _MHManager.getSteamID() << std::endl;
        std::cout << "\tSteam Game Directory: " << _MHManager.getSteamPath() << std::endl;
    }
    else
        std::cout << "\tCouldn't Find Steam Data" << std::endl;
}


/// Begin About Window Member definitions

AboutWindow::AboutWindow(QWidget *parent) : QDialog(parent), ui(new Ui::AboutWindow)
{
    ui->setupUi(this);

    std::string version = ui->version->text().toUtf8().constData();
    version.replace(version.find("MAJOR"), 5, std::to_string(PROJECT_VERSION_MAJOR));
    version.replace(version.find("MINOR"), 5, std::to_string(PROJECT_VERSION_MINOR));
    version.replace(version.find("PATCH"), 5, std::to_string(PROJECT_VERSION_PATCH));

    ui->version->setText(version.c_str());
    ui->description->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->description->setOpenExternalLinks(true);

    connect(ui->closeButton, SIGNAL(released()), this, SLOT(accept()));
}
AboutWindow::~AboutWindow()
{
    delete ui;
}

/// Begin Dialog Message Member definitions

DialogWindow::DialogWindow(QWidget *parent, const std::string &Title, const std::string &Msg, int status) :
QDialog(parent), ui(new Ui::DialogWindow)
{   
    if (status > Status::ERROR0 || status < Status::SUCCESS)
        status = Status::ERROR0;

    ui->setupUi(this);

    std::string statusIcon = ui->_iconLabel->text().toUtf8().constData();
    statusIcon.replace(statusIcon.find("<++>"), 4, Status::Names[status]);
    ui->_iconLabel->setText(statusIcon.c_str());

    this->setWindowTitle(Title.c_str());

    ui->_Message->setText(Msg.c_str());

    connect(ui->_okButton, SIGNAL(released()), this , SLOT(accept()));

    this->setAttribute(Qt::WA_DeleteOnClose);
}

DialogWindow::~DialogWindow()
{
    delete ui;
}