#pragma once
#include <QMainWindow>
#include "ui_AboutWindow.h"
#include "ui_Instructions.h"

namespace Ui {
class AboutWindow;
class Instructions;
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

class Instructions : public QDialog
{
    Q_OBJECT
  public:
    explicit Instructions(QWidget *parent = 0);
    ~Instructions();

  private:
    Ui::Instructions *ui;
};
