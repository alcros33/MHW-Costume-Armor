#include "DialogWindow.hpp"
#include <QDialogButtonBox>
#include <iostream>
/// Begin Dialog Message Member definitions

DialogWindow::DialogWindow(QWidget *parent, const std::string &Title, const std::string &Msg, int status) :
QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint),
ui(new Ui::DialogWindow)
{   
    if (status > Status::ERROR0 || status < Status::SUCCESS)
        status = Status::ERROR0;

    ui->setupUi(this);

    QString statusIcon = ui->_iconLabel->text();
    statusIcon = statusIcon.arg(Status::Names[status]);
    ui->_iconLabel->setText(statusIcon);

    this->setWindowTitle(Title.c_str());
    ui->_Message->setText(Msg.c_str());

    connect(ui->_okButton, QPushButton::released, this, accept);

    this->setAttribute(Qt::WA_DeleteOnClose,true);
}

DialogWindow::~DialogWindow()
{
    delete ui;
}

QString getTextInputDialog(QWidget *parent, const std::string &Title, const std::string &Message, bool *ok)
{
    return QInputDialog::getText(parent, Title.c_str(), Message.c_str(), QLineEdit::Normal, "", ok, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);
}

QString getItemInputDialog(QWidget *parent, const std::string &Title, const std::string &Message, const QStringList &items, bool *ok)
{
    return QInputDialog::getItem(parent, Title.c_str(), Message.c_str(), items, 0, false, ok, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);
}