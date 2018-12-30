#include "MainWindow.hpp"
#include "Config.h"
#include <QThread>
#include <QMovie>
#include <QXmlStreamReader>
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
    connect(ui->actionTutorial, SIGNAL(triggered()), this, SLOT(_Instructions()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()) );

    connect(ui->SearchButton, SIGNAL(released()), this, SLOT(_FindAddr()));
    connect(ui->FetchButton, SIGNAL(released()), this, SLOT(_FetchData()));
    connect(ui->WriteButton, SIGNAL(released()), this, SLOT(_WriteData()));

    connect(ui->actionSave_Current_Armor, SIGNAL(triggered()), this, SLOT(_NotImplemented()));
    connect(ui->actionManaged_Saved_Sets, SIGNAL(triggered()), this, SLOT(_NotImplemented()));
    connect(ui->actionLoad_Armor, SIGNAL(triggered()), this, SLOT(_NotImplemented()));

    _InputBoxes[0] = ui->headLineEdit ;
    _InputBoxes[1] = ui->bodyLineEdit ;
    _InputBoxes[2] = ui->armsLineEdit ;
    _InputBoxes[3] = ui->waistLineEdit;
    _InputBoxes[4] = ui->legsLineEdit ;
    //load item details here
    QFile f("armor.xml");
    QXmlStreamReader reader(&f);
    reader.readNextStartElement()
    while (reader.readNextStartElement()) {
        _data[0]=reader.readNextStartElement().readElementText();
        for(i=0; i<5; i++){
            ii=i+1;
            _data[ii]=reader.readNextStartElement().readElementText();
            if(_data[ii]!="?"){
                _InputBoxes[i]->addItem(_data[ii], QVariant(_data[0]));
            }
        }
    }
}

void MainWindow::show()
{
    QMainWindow::show();
    if (!this->_MHManager.ProcessOpen())
    {
        DialogWindow *Dia = new DialogWindow(nullptr, "ERROR", "MHW is Closed\nPlease open it before starting.", Status::ERROR0);
        Dia->getOkButton()->setStyleSheet("");
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

void MainWindow::_Instructions()
{
    Instructions *Dia = new Instructions(this);
    Dia->show();
}

void MainWindow::_FindAddr()
{
    ui->SearchButton->setText("Searching for Save Data...");
    ui->SearchButton->setEnabled(false);

    DialogWindow *Dia = new DialogWindow(this, "Wait a Sec...", "Searching MHW for Character Data", Status::SUCCESS);
    Dia->setWindowFlags(Qt::SplashScreen);
    QLabel *IconLabel = Dia->getIconLabel();
    std::string PrevText = IconLabel->text().toUtf8().constData();
    QMovie *movie = new QMovie(":/ajax-loader.gif");
    movie->setScaledSize(IconLabel->frameSize());
    IconLabel->setMovie(movie);
    movie->start();

    Dia->getOkButton()->setEnabled(false);
    Dia->setAttribute(Qt::WA_DeleteOnClose, false);

    Dia->show();

    QThread *thread = QThread::create( [this] {this->_MHManager.FindAddress(); });
    thread->start();

    while (thread->isRunning() )
    {
        QCoreApplication::processEvents();
    }

    if (!this->_MHManager.DataAddressFound())
    {
        Dia->setAttribute(Qt::WA_DeleteOnClose, true);
        Dia->close();
        ui->SearchButton->setEnabled(true);
        DialogWindow *Dia = new DialogWindow(this, "ERROR", "Couldn't Find Save Data Address", Status::ERROR0);
        Dia->show();
        ui->SearchButton->setText("Search For MHW Save Data");
        return;
    }
    Dia->getOkButton()->setEnabled(true);
    Dia->getIconLabel()->clear();
    Dia->getIconLabel()->setText(PrevText.c_str());
    Dia->getMsgLabel()->setText("MHW Data Found Successfully!");
    Dia->setWindowTitle("Succes!!");
    Dia->setAttribute(Qt::WA_DeleteOnClose, true);

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
            std::string strVal = _InputBoxes[i]->itemData(index).toUtf8().constData();

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
        DEBUG_LOG( "\tSteam UserData ID: " << _MHManager.getSteamID() );
        DEBUG_LOG( "\tSteam Game Directory: " << _MHManager.getSteamPath() );
    }
    else
    {
        DEBUG_LOG( "\tCouldn't Find Steam Data" );
    }
}

void MainWindow::_NotImplemented()
{
    DialogWindow *Dia = new DialogWindow(this, "Warning", "Functionality Not Implemented... Yet", Status::WARNING);
    Dia->show();
}
