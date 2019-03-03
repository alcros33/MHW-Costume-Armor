#include "AboutWindow.hpp"
#include "Config.h"

/// Begin About Window Member definitions

AboutWindow::AboutWindow(QWidget *parent) :
QDialog(parent,Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint),
ui(new Ui::AboutWindow)
{
    ui->setupUi(this);

    std::string version = ui->version->text().toUtf8().constData();
    version.replace(version.find("MAJOR"), 5, std::to_string(PROJECT_VERSION_MAJOR));
    version.replace(version.find("MINOR"), 5, std::to_string(PROJECT_VERSION_MINOR));
    version.replace(version.find("PATCH"), 5, std::to_string(PROJECT_VERSION_PATCH));

    ui->version->setText(version.c_str());
    ui->description->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->description->setOpenExternalLinks(true);

    connect(ui->closeButton, QPushButton::released, this, accept);
    this->setAttribute(Qt::WA_DeleteOnClose, true);
}
AboutWindow::~AboutWindow()
{
    delete ui;
}

/// Begin Instructions Member definitions

Instructions::Instructions(QWidget *parent) :
QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint),
ui(new Ui::Instructions)
{
    ui->setupUi(this);
    
    ui->description->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->description->setOpenExternalLinks(true);

    connect(ui->closeButton, QPushButton::released, this, accept);
    this->setAttribute(Qt::WA_DeleteOnClose, true);
}
Instructions::~Instructions()
{
    delete ui;
}