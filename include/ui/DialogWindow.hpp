#pragma once
#include <QObject>
#include <QMainWindow>
#include "ui_DialogWindow.h"

#include <QInputDialog>
#include <array>
#include <string>
#include <tuple>

namespace Status
{
    const int SUCCESS = 0;
    const int WARNING = 1;
    const int ERROR0 = 2;
    const std::array<QString,3> Names{"success","warning","error"};
} // namespace Status

namespace Ui {
class DialogWindow;
}

class DialogWindow : public QDialog
{
    Q_OBJECT
  public:
    DialogWindow(QWidget *parent, const QString &Title, const QString &Message, int Status);
    ~DialogWindow();

    QLabel* getIconLabel() { return ui->_iconLabel; }
    QPushButton* getOkButton() { return ui->_okButton; }
    QLabel *getMsgLabel() { return ui->_Message; }

  private:
    Ui::DialogWindow *ui;
};

QString getTextInputDialog(QWidget* parent, const QString &Title, const QString &Message, bool* ok);
// QString getItemInputDialog(QWidget* parent, const QString &Title, const QString &Message, const QStringList &items, bool* ok);
std::tuple<QString, unsigned int, bool>
getItemInputDialog(QWidget *parent, const QString &title, const QString &message, const QStringList &items, const QList<uint> &datas);