#include "AboutWindow.hpp"
#include "Config.h"

/// Begin About Window Member definitions

AboutWindow::AboutWindow(QWidget *parent, float font_size) :
QDialog(parent,Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint),
ui(new Ui::AboutWindow)
{
    ui->setupUi(this);

    QString version = ui->version->text();
    version = version.arg(PROJECT_VERSION_MAJOR).arg(PROJECT_VERSION_MINOR).arg(PROJECT_VERSION_PATCH);
    ui->version->setText(version);
    
    ui->description->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->description->setOpenExternalLinks(true);

    static const auto qssTemplate = QStringLiteral("QLabel{font-size:%1pt;}"
        "#description, #thanks, #version {font-size : %2pt; }"
        "QLabel#title{font-size : %3pt;}");
    this->setStyleSheet(qssTemplate.arg(font_size*1.25).arg(font_size*1.5).arg(font_size*2.25));

    connect(ui->closeButton, QPushButton::released, this, accept);

    this->setAttribute(Qt::WA_DeleteOnClose, true);
}
AboutWindow::~AboutWindow()
{
    delete ui;
}