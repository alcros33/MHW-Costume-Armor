#pragma once
#include <QMainWindow>
#include "ui_DialogWindow.h"

#include <QInputDialog>

namespace Status
{
    const int SUCCESS = 0;
    const int WARNING = 1;
    const int ERROR0 = 2;
    const std::array<std::string,3> Names{"success","warning","error"};
} // namespace Status

namespace Ui {
class DialogWindow;
}

class DialogWindow : public QDialog
{
    Q_OBJECT
  public:
    DialogWindow(QWidget *parent, const std::string &Title, const std::string &Message, int Status);
    ~DialogWindow();

    QLabel* getIconLabel() { return ui->_iconLabel; }
    QPushButton* getOkButton() { return ui->_okButton; }
    QLabel *getMsgLabel() { return ui->_Message; }

  private:
    Ui::DialogWindow *ui;
};

QString getTextInputDialog(QWidget *parent, const std::string &Title, const std::string &Message, bool *ok);
QString getItemInputDialog(QWidget *parent, const std::string &Title, const std::string &Message, const QStringList &items, bool *ok);