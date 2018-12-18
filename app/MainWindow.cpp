#include "MainWindow.hpp"
#include "Config.h"

/// Begin Main Window Member definitions

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("MHW Mk. Armor");
    ui->FetchButton->setEnabled(false);
    ui->WriteButton->setEnabled(false);

    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(_aboutInfo()));
    
    if (!this->_MHManager.ProcessOpen())
    {
        DialogMessage *Dia = new DialogMessage(this, "ERROR", "MHW is Closed,Open it and restart.");
        Dia->show();
        ui->SearchButton->setText("Plz Restart Application");
        ui->SearchButton->setEnabled(false);
        return;
    }

    connect(ui->SearchButton, SIGNAL(released()), this, SLOT(_FindAddr()));
    connect(ui->FetchButton, SIGNAL(released()), this, SLOT(_FetchData()));
    connect(ui->WriteButton, SIGNAL(released()), this, SLOT(_WriteData()));
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
        DialogMessage *Dia = new DialogMessage(this, "ERROR", "Couldn't Find Save Data Address");
        Dia->show();
        ui->SearchButton->setText("Search For MHW Save Data");
        return;
    }

    ui->SearchButton->setText("Ready To Go.");
    ui->SearchButton->setEnabled(false);

    ui->FetchButton->setEnabled(true);
    ui->WriteButton->setEnabled(true);
}

void MainWindow::_FetchData()
{
    int slot = std::stoi(ui->comboBox->currentText().toUtf8().constData());
    if (!_MHManager.FetchPlayerData(slot-1))
    {
        DialogMessage *Dia = new DialogMessage(this, "ERROR", "Couldn't Fetch Character Data...");
        Dia->show();
        return;
    }
    auto Data = _MHManager.getPlayerData().getDataString();

    ui->headLineEdit->setText(Data[0].c_str());
    ui->bodyLineEdit->setText(Data[1].c_str());
    ui->armsLineEdit->setText(Data[2].c_str());
    ui->waistLineEdit->setText(Data[3].c_str());
    ui->legsLineEdit->setText(Data[4].c_str());

    std::string msg = "Sucessfully Character Data for slot " + std::to_string(slot);
    DialogMessage *Dia = new DialogMessage(this, "Sucess", msg);
    Dia->show();
}

void MainWindow::_WriteData()
{
    DialogMessage *Dia = new DialogMessage(this, "Warning", "Not implemented...yet");
    Dia->show();
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

DialogMessage::DialogMessage(QWidget *parent, const std::string &Title, const std::string &Msg) : QDialog(parent), _Message(this), _okButton(this)
{
    this->resize(400, 100);
    this->setWindowTitle(Title.c_str());

    this->_Message.setGeometry(QRect(50, 20, 350, 20));
    this->_Message.setAlignment(Qt::AlignCenter);
    this->_Message.setObjectName("Message");
    this->_Message.setText(Msg.c_str());

    this->_okButton.setGeometry(QRect(150, 60, 100, 30));
    this->_okButton.setObjectName("okButton");
    this->_okButton.connect(&this->_okButton, SIGNAL(released()), this, SLOT(accept()));
    this->_okButton.setText("Ok");
}