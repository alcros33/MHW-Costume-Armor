#pragma once
#include <QMainWindow>
#include "ui_AboutWindow.h"

namespace Ui {
class AboutWindow;
}

class AboutWindow : public QDialog
{
    Q_OBJECT
  public:
    explicit AboutWindow(QWidget *parent = 0);
    ~AboutWindow();

  private:
    Ui::AboutWindow *ui;
};
