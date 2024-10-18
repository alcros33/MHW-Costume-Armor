#include "MainWindow.hpp"
#include "Config.h"
#include <QThread>
#include <QMovie>
#include <QInputDialog>
#include <QFileDialog>
#include <QActionGroup>
#include <QResizeEvent>

/// These file contains Member definitions of the MainWindow class
/// Related to handle settings

void MainWindow::_updateSelectedVersion()
{
    QAction *checked = _versionGroup->checkedAction();
    _settings.setValue("General/GameVersion",checked->text());

    QFont font;
    for (auto verAction : _versionGroup->actions())
        verAction->setFont(font);

    font.setBold(true);
    checked->setFont(font);
    this->_settings.sync();
}

void MainWindow::_updateSelectedLogLevel()
{
    QAction *checked = _logGroup->checkedAction();
    _settings.setValue("Debug/LogLevel", checked->text());
    GLOBAL_LOG_LEVEL = checked->text();

    QFont font;
    for (auto act : _logGroup->actions())
        act->setFont(font);

    font.setBold(true);
    checked->setFont(font);
    this->_settings.sync();
}

void MainWindow::_toggleNoBackup()
{
    _settings.setValue("General/NoBackupOk", !_settings.value("General/NoBackupOk",true).toBool());
    _settings.sync();
}

void MainWindow::_getCustomSteamPath()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select the folder", "C:\\", QFileDialog::ShowDirsOnly);
    if (dir.isEmpty())
        return;
    _settings.setValue("SteamPath/CustomPath", dir);
    _settings.setValue("SteamPath/Auto", false);
    ui->actionAutoSteam->setChecked(false);
    this->_MHManager.setSteamDirectory(dir);
    ui->actionSteam_Current->setText(dir.prepend("Current : "));
    this->_settings.sync();
}

void MainWindow::_setAutoSteam()
{
    if (_settings.value("SteamPath/Auto", true).toBool())
    {
        ui->actionAutoSteam->setChecked(true);
        return;
    }

    _settings.setValue("SteamPath/Auto", true);
    this->_settings.sync();
    this->_MHManager.unSetSteamDirectory();
    if (!this->_MHManager.processIsOpen())
    {
        auto Dia = new DialogWindow(this, "Automatic Steam Path", "Steam path has been set to automatic, but MHW seems to be closed. Open it and press Search MHW Character Data.", Status::WARNING);
        Dia->show();
        return;
    }
    this->_MHManager.findSteamPath();
}

void MainWindow::_toggleAutoUpdates()
{
    _settings.setValue("SteamPath/Auto", !_settings.value("SteamPath/Auto", true).toBool());
    this->_settings.sync();
}

void MainWindow::_setFontSize()
{
    auto f = _settings.value("General/FontBaseSize", 8.0).toDouble();
    static const auto qssTemplate = QStringLiteral(
        "QWidget {font-size:%1pt;}\n"
        "QMenuBar, QMenuBar *{font-size:9pt;}\n"
        "QDialog *, QPushButton#SearchButton{font-size:%2pt;}\n"
        "QToolBar *, QPushButton, #centralwidget > QLabel, #centralwidget > QComboBox"
        "{font-size : %3pt;}\n"
        "QLabel#Title{font-size : %4pt;}\n");
    // "#centralwidget > QPushButton{border-radius : 60px;}");
    auto currQSS = this->styleSheet();
    currQSS.resize(_qssLen);
    this->setStyleSheet(currQSS.append(
        qssTemplate.arg(f).arg(f*1.25).arg(f*1.5).arg(f*3.25)
        ));
    static const auto szTP = QStringLiteral("FontSize(%1)");
    ui->actionFont_Size->setText(szTP.arg(f));
}

void MainWindow::_fontScale()
{
    bool ok;
    float f = QInputDialog::getDouble(this, QStringLiteral("Enter base size of font"),
                                     QStringLiteral("Base Size"),
                                     _settings.value("General/FontBaseSize", 8.F).toFloat(),
                                     1.0, 1000.0, 1, &ok);
    if (!ok)
        return;
    _settings.setValue("General/FontBaseSize", QString::number(f));
    _settings.sync();
    _setFontSize();
}

bool MainWindow::_ensureSizeRatio(QResizeEvent *event)
{
    // has desired ratio
    return false;
}

void MainWindow::_resizeWidgets()
{
    float scaleF = _settings.value("General/WindowScaleFactor", 1.0F).toFloat();
    for (auto c : _childCache)
    {
        auto &rec = _baseGeo[c];
        c->resize(rec.first * scaleF);
        c->move(rec.second * scaleF);
    }
    ui->savedComboBox->setMinimumSize(_baseGeo[ui->savedComboBox].first.width() * scaleF,
                                      ui->savedComboBox->height());
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    int desiredWidth = (16.F * event->size().height()) / 10.F;
    int dist = event->size().width() - desiredWidth;

    if (dist >= -3 && dist <= 3)
        return;
    // make desired ratio
    if (event->size().width() == event->oldSize().width())
        this->resize((16.F * event->size().height()) / 10.F, event->size().height());
    else //
        this->resize(event->size().width(), (10.F * event->size().width()) / 16.F);
    
    // resize everything
    _settings.setValue("General/WindowScaleFactor", QString::number((float)this->size().width() / 1040.F));
    _settings.sync();
    _resizeWidgets();
}