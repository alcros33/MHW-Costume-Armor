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

    std::string statusIcon = ui->_iconLabel->text().toUtf8().constData();
    statusIcon.replace(statusIcon.find("<++>"), 4, Status::Names[status]);
    ui->_iconLabel->setText(statusIcon.c_str());

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
    QInputDialog *Dia = new QInputDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    Dia->setInputMode( QInputDialog::TextInput);
    Dia->setWindowTitle(Title.c_str());                        
    Dia->setLabelText(Message.c_str());
    Dia->resize(300,150);
    Dia->setAttribute(Qt::WA_DeleteOnClose, true);

    QLineEdit *edit = Dia->findChild<QLineEdit *>();
    if (edit!=0)
        edit->setAlignment(Qt::AlignCenter);

    Dia->setStyleSheet("QWidget{"
                        "font: 75 10pt \"MS Shell Dlg 2\";"
                        "}");

    (*ok) = Dia->exec();                                
    return Dia->textValue();
}

QString getItemInputDialog(QWidget *parent, const std::string &Title, const std::string &Message, const QStringList &items, bool *ok)
{
    QInputDialog *Dia = new QInputDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    Dia->setInputMode( QInputDialog::TextInput);
    Dia->setWindowTitle(Title.c_str());                        
    Dia->setLabelText(Message.c_str());
    Dia->resize(300,150);
    Dia->setAttribute(Qt::WA_DeleteOnClose, true);
    Dia->setComboBoxItems(items);

    Dia->setStyleSheet("QWidget{"
                        "font: 75 10pt \"MS Shell Dlg 2\";"
                        "}");

    (*ok) = Dia->exec();                                
    return Dia->textValue();
}
