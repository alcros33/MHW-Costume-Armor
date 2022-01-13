#include "DialogWindow.hpp"
#include <QDialogButtonBox>
#include <QComboBox>
#include <iostream>
/// Begin Dialog Message Member definitions

DialogWindow::DialogWindow(QWidget *parent, const QString &Title, const QString &Msg, int status) :
QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint),
ui(new Ui::DialogWindow)
{   
    if (status > Status::ERROR0 || status < Status::SUCCESS)
        status = Status::ERROR0;

    ui->setupUi(this);

    QString statusIcon = ui->_iconLabel->text();
    statusIcon = statusIcon.arg(Status::Names[status]);
    ui->_iconLabel->setText(statusIcon);

    this->setWindowTitle(Title);
    ui->_Message->setText(Msg);

    connect(ui->_okButton, QPushButton::released, this, accept);

    this->setAttribute(Qt::WA_DeleteOnClose,true);
}

DialogWindow::~DialogWindow()
{
    delete ui;
}

QString getTextInputDialog(QWidget *parent, const QString &Title, const QString &Message, bool *ok)
{
    return QInputDialog::getText(parent, Title, Message, QLineEdit::Normal, "", ok, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);
}

std::tuple<QString, uint, bool>
getItemInputDialog(QWidget *parent, const QString &title, const QString &message, const QStringList &items, const QList<uint> &datas)
{
    auto text = items.value(0);
    auto dialog = new QInputDialog(parent,
                                    Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);
    dialog->setWindowTitle(title);
    dialog->setLabelText(message);
    dialog->setComboBoxItems(QStringList());

    auto cB = dialog->findChild<QComboBox *>();
    {
        const QSignalBlocker blocker(cB);
        cB->clear();
        for (int i = 0; i < items.size(); i++)
        {
            cB->addItem(items[i], datas[i]);
        }
    }
    dialog->setTextValue(text);
    dialog->setComboBoxEditable(false);
    dialog->setInputMethodHints(Qt::ImhNone);
    cB->model()->sort(0);

    const int ret = dialog->exec();
    bool ok = !!ret;
    if (ret)
        return std::make_tuple(dialog->textValue(), cB->currentData().toUInt(), ok);
    else
        return std::make_tuple(text, 0, ok);
}