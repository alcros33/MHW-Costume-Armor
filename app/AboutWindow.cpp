#include "AboutWindow.hpp"
#include "Config.h"

/// Begin About Window Member definitions

AboutWindow::AboutWindow(QWidget *parent) :
QDialog(parent,Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint),
ui(new Ui::AboutWindow)
{
    ui->setupUi(this);

    QString version = ui->version->text();
    version = version.arg(PROJECT_VERSION_MAJOR).arg(PROJECT_VERSION_MINOR).arg(PROJECT_VERSION_PATCH);
    ui->version->setText(version);
    
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
    connect(ui->closeButton, QPushButton::released, this, accept);
    this->setAttribute(Qt::WA_DeleteOnClose, true);
}
Instructions::~Instructions()
{
    delete ui;
}