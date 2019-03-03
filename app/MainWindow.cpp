#include "MainWindow.hpp"
#include "Config.h"
#include <QThread>
#include <QMovie>
#include <QInputDialog>
#include <QFileDialog>
#include <QActionGroup>

/// Begin Main Window Member definitions

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(PROJECT_NAME);
    ui->FetchButton->setEnabled(false);
    ui->WriteButton->setEnabled(false);

    connect(ui->actionAbout, QAction::triggered, this, _aboutInfo);
    connect(ui->actionTutorial, QAction::triggered, this, _Instructions);
    connect(ui->actionSafe_Mode, QAction::triggered, this, _ToggleSafe);
    connect(ui->actionExit, QAction::triggered, this, close);

    connect(ui->SearchButton, QPushButton::released, this, _FindAddr);
    connect(ui->FetchButton, SIGNAL(released()), this, SLOT(_FetchData()));
    connect(ui->WriteButton, QPushButton::released, this, _WriteData);
    connect(ui->ClearButton, QPushButton::released, this, _ClearArmor);
    connect(ui->ChangeAllButton, QPushButton::released, this, _ChangeAll);

    connect(ui->actionSave_Current_Armor, QAction::triggered, this, _SaveCurrentSet);
    connect(ui->actionLoad_Armor, QAction::triggered, this, _LoadSavedSet);
    connect(ui->actionChange_Steam_Path, QAction::triggered, this, _GetCustomSteamPath);

    connect(ui->actionManaged_Saved_Sets, QAction::triggered, this, _NotImplemented);

    _InputBoxes[0] = ui->headEdit ;
    _InputBoxes[1] = ui->bodyEdit ;
    _InputBoxes[2] = ui->armsEdit ;
    _InputBoxes[3] = ui->waistEdit;
    _InputBoxes[4] = ui->legsEdit ;

    this->_ArmorDataFound = _LoadConfigFiles();

    if (!_ArmorDataFound || !this->_MHManager.ProcessOpen())
        return;

    if (_Settings.find("Safe Mode") == _Settings.end())
        _Settings["Safe Mode"] = true;

    if (_Settings.find("Game Version") == _Settings.end())
        _Settings["Game Version"] = "Latest";
    
    if (_Settings.find("Steam Path") != _Settings.end())
    {
        std::string SteamPath = _Settings["Steam Path"].get<std::string>();
        _MHManager.setSteamDirectory(SteamPath);
        ui->actionSteam_Current->setText(("Current : " + SteamPath).c_str());
    }

    _PopulateVersionSelector();

    _PopulateComboBoxes();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::show()
{
    if (!this->_ArmorDataFound)
    {
        DEBUG_LOG(ERROR, "Armor Data json file searched at " << ArmorDataFile<< " and not found");
        DialogWindow *Dia = new DialogWindow(nullptr, "ERROR FATAL", "Couldn't find " +
            ArmorDataFile.filename().string() +
            "\nIf It was deletd, re-download the program.", Status::ERROR0);
        Dia->getOkButton()->setStyleSheet("");
        Dia->show();
        this->close();
    }
    else if (!this->_MHManager.ProcessOpen())
    {
        DialogWindow *Dia = new DialogWindow(nullptr, "ERROR", "MHW is Closed\nPlease open it before starting.", Status::ERROR0);
        Dia->getOkButton()->setStyleSheet("");
        Dia->show();
        this->close();
    }
    else
    {
        QMainWindow::show();
        if (!_Settings["Safe Mode"])
        {
            this->_UnsafeWarning();
            ui->actionSafe_Mode->setChecked(false);
        }
    }
}

bool MainWindow::_LoadConfigFiles()
{
    if (fs::exists(SettingsFile))
    {
        std::ifstream i(SettingsFile);
        i >> _Settings;
    }
    if (fs::exists(SavedSetsFile))
    {
        std::ifstream i(SavedSetsFile);
        i >> _SavedSets;
    }
    if(!fs::exists(ArmorDataFile))
        return false;
    
    std::ifstream i(ArmorDataFile);
    i >> _ArmorData;
    return true;
}

void MainWindow::_PopulateVersionSelector()
{
    QMenu *menuVersions = new QMenu(ui->menuOptions);
    menuVersions->setObjectName(QString::fromUtf8("menu_Versions"));
    menuVersions->setTitle("Select Game Version");
    ui->menuOptions->addAction(menuVersions->menuAction());

    _VersionGroup = new QActionGroup(this);

    QFont font;
    font.setBold(true);

    _VersionActions.reserve(MH_Memory::Versions.size());
    std::string TargetVersion = _Settings["Game Version"];
    for (auto it=MH_Memory::Versions.begin(); it!=MH_Memory::Versions.end(); ++it)
    {
        QAction *tmp = new QAction(this);
        connect(tmp, QAction::triggered, this, _UpdateSelectedVersion);
        tmp->setText(it->first.c_str());
        tmp->setCheckable(true);
        menuVersions->addAction(tmp);
        _VersionGroup->addAction(tmp);
        if (it->first == TargetVersion)
        {
            tmp->setChecked(true);
            tmp->setFont(font);
        }
        _VersionActions.push_back(tmp);
    }
    if (_VersionGroup->checkedAction() == 0)
    {
        DEBUG_LOG(ERROR, "Encountered unknown game version " << TargetVersion << " setting it to 'Latest'");
        _VersionActions.back()->setChecked(true);
        _VersionActions.back()->setFont(font);
    }
}

void MainWindow::_UpdateSelectedVersion()
{
    QAction *checked = _VersionGroup->checkedAction();
    _Settings["Game Version"] = checked->text().toStdString();

    QFont font;
    for(auto verAction : _VersionActions)
        verAction->setFont(font);
    
    font.setBold(true);
    checked->setFont(font);
    this->_FlushSettings();
}

void MainWindow::_PopulateComboBoxes()
{
    int i;
    int ID;

    for (i = 0; i < 5; ++i)
        this->_InputBoxes[i]->addItem("Nothing", 255);

    for (auto &el : _ArmorData.items())
    {
        ID = el.value()["ID"];
        
        if (el.value()["Danger"])
        {
            _UnSafeArmors.insert(el.key());

            if (_Settings["Safe Mode"])
                continue;
        }

        for(i=0; i<5; ++i)
        {
            if (el.value()[Armor::Names[i]])
                this->_InputBoxes[i]->addItem(el.key().c_str(), ID);
        }
    }
    for (i = 0; i < 5; ++i)
        _SafeCount[i] = this->_InputBoxes[i]->count();
}

void MainWindow::_ToggleSafe()
{
    _Settings["Safe Mode"] = !_Settings["Safe Mode"];
    if (!_Settings["Safe Mode"])
    {
        this->_UnsafeWarning();
    }
    else
    {
        this->_DeleteUnsafe();
        this->setWindowTitle(PROJECT_NAME);
    }
    _FlushSettings();
}

void MainWindow::_UnsafeWarning()
{
    if (_Settings.find("Disable Unsafe Warning") != _Settings.end() )
        if (_Settings["Disable Unsafe Warning"] == true )
            return;

    this->setWindowTitle( QString(PROJECT_NAME) + "-UNSAFE" );
    DEBUG_LOG(WARNING,"Safe Mode was turned off");
    DialogWindow *Dia = new DialogWindow(this, "Warning", "You are not Running in SafeMode\n(!) Marked Armors May Cause Game Crashes\nUse with caution.", Status::WARNING);
    Dia->show();
    this->_AddUnsafe();
}

void MainWindow::_FindAddr()
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

    QThread *thread = QThread::create([this] { this->_MHManager.FindAddress(this->_Settings["Game Version"]); });

    try
    {
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
    }
    catch (std::system_error &e) // VritualQueryEx returned error code 0
    {
        Dia->setAttribute(Qt::WA_DeleteOnClose, true);
        Dia->close();
        ui->SearchButton->setText("Search For MHW Save Data");
        ui->SearchButton->setEnabled(true);
        DEBUG_LOG(ERROR, "VritualQueryEx returned error code 0");
        DialogWindow *Dia = new DialogWindow(this, "ERROR",
        "Error occurred during Searching process.\nConsider running the program as Adminstrator.", Status::ERROR0);
        Dia->show();
        return;
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

void MainWindow::_FetchData(bool noMessage)
{
    int slot = std::stoi(ui->comboBox->currentText().toStdString() );
    if (!_MHManager.FetchPlayerData(slot-1))
    {
        DialogWindow *Dia = new DialogWindow(this, "ERROR", "Couldn't Fetch Character Data...", Status::ERROR0);
        Dia->show();
        return;
    }
    this->_UpdateArmorValues();
    std::string msg = "Successfully fetched Data for Character Slot " + std::to_string(slot);
    if(!noMessage)
    {
        DialogWindow *Dia = new DialogWindow(this, "Success!!", msg, Status::SUCCESS);
        Dia->show();
    }
}

void MainWindow::_LoadSavedSet()
{
    if (_SavedSets.empty())
    {
        DialogWindow *Dia = new DialogWindow(this, "Warning", "There are no saved sets", Status::WARNING);
        Dia->show();
        return;
    }

    QStringList items;
    for (const auto &it : _SavedSets.items() )
        items << it.key().c_str();
    
    bool ok;
    QString text = getItemInputDialog(this, "Select Armor Set", "Select set: ", items, &ok);
    if (!ok)
        return;
    
    try
    {
        for(int i=0; i<5 ;++i)
            _MHManager.getPlayerData().setArmorPiece(i, _SavedSets[text.toStdString()][i] );
    }
    catch (std::exception &e)
    {
        DialogWindow *Dia = new DialogWindow(this, "ERROR", "Invalid Value for armor", Status::ERROR0);
        Dia->show();
        DEBUG_LOG(ERROR,"Invalid value at json file. Error "<<e.what() );
        return;
    }
    this->_UpdateArmorValues();
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

void MainWindow::_SaveCurrentSet()
{
    if (!this->_ParseInputBoxes())
        return;

    bool ok;
    QString text;
    while (true)
    {
        text = getTextInputDialog(this, "Saving Armor Set...", "Name for the set: ",&ok);
        if (!ok)
            return;

        if (_SavedSets.find(text.toStdString() ) != _SavedSets.end())
        {
            DialogWindow *Dia = new DialogWindow(this, "WARNING", "The name already exists.", Status::WARNING);
            Dia->show();
            while (Dia->isVisible())
                QCoreApplication::processEvents();
            continue;
        }

        if(!text.isEmpty())
            break;
    }

    auto Data = _MHManager.getPlayerData().getData();
    _SavedSets[text.toStdString()] = Data;

    if(!this->_FlushSavedSets())
    {
        DialogWindow *Dia = new DialogWindow(this, "ERROR", "Couldn't save set.", Status::ERROR0);
        Dia->show();
        return;
    }
    DialogWindow *Dia = new DialogWindow(this, "Success", "Successfully saved set!", Status::SUCCESS);
    Dia->show();
}

void MainWindow::_WriteData()
{
    if (!this->_ParseInputBoxes())
        return;

    int slot = std::stoi(ui->comboBox->currentText().toStdString() );
    if (!_MHManager.WriteArmor(slot - 1, _Settings["Safe Mode"]))
    {
        DialogWindow *Dia = new DialogWindow(this, "ERROR", "Couldn't Write Save Data!", Status::ERROR0);
        Dia->show();
    }
    else
    {
        DialogWindow *Dia = new DialogWindow(this, "Success!!", "Success writing to Game!!"
                "\nYou may preview the appearance by checking your Guild Card"
                "\nBut be sure to reload by entering another area.", Status::SUCCESS);
        Dia->show();
    }
    this->_FetchData(true);
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

bool MainWindow::_FlushSavedSets()
{
    std::ofstream Out(SavedSetsFile.c_str());
    if (!Out)
    {
        DEBUG_LOG(ERROR, "Couldn't open " << SavedSetsFile);
        Out.close();
        return false;
    }
    Out << std::setw(2) << _SavedSets << std::endl;
    Out.close();
    return true;
}

bool MainWindow::_FlushSettings()
{
    std::ofstream Out(SettingsFile.c_str());
    if (!Out)
    {
        DEBUG_LOG(ERROR, "Couldn't open " << SavedSetsFile);
        Out.close();
        return false;
    }
    Out << std::setw(2) << _Settings << std::endl;
    Out.close();
    return true;
}

void MainWindow::_GetCustomSteamPath()
{
    QString Dir = QFileDialog::getExistingDirectory(this, "Open Folder Containing Steam.exe", "C:\\", QFileDialog::ShowDirsOnly);
    if (Dir.isEmpty())
        return;
    _Settings["Steam Path"] = Dir.toStdString();
    this->_MHManager.setSteamDirectory(Dir.toStdString());
    ui->actionSteam_Current->setText(("Current : " + Dir.toStdString()).c_str());
    this->_FlushSettings();
}

void MainWindow::debugPrints() const
{
    if (_MHManager.SteamFound())
    {
        DEBUG_LOG(DEBUG, "Steam UserData ID: " << _MHManager.getSteamID() );
        DEBUG_LOG(DEBUG, "Steam Game Directory: " << _MHManager.getSteamPath() );
    }
    else
    {
        DEBUG_LOG(ERROR, "Couldn't Find Steam Data" );
    }
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

void MainWindow::_NotImplemented() 
{
    DialogWindow *Dia = new DialogWindow(this, "Warning", "Functionality Not Implemented... Yet", Status::WARNING);
    Dia->show();
}
