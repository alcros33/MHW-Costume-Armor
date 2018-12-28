#include "DialogWindow.hpp"

/// Begin Dialog Message Member definitions

DialogWindow::DialogWindow(QWidget *parent, const std::string &Title, const std::string &Msg, int status) :
QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint),
ui(new Ui::DialogWindow)
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

    this->setAttribute(Qt::WA_DeleteOnClose,true);
}

DialogWindow::~DialogWindow()
{
    delete ui;
}