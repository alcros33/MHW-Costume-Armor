#pragma once
#include <QMainWindow>

#include "ui_MainWindow.h"
#include "ui_AboutWindow.h"

#include "MHMemory.hpp"
#include "json.hpp"

// for convenience
using json = nlohmann::json;

namespace Ui {
class MainWindow;
class AboutWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void debugPrints() const ;

private:
    Ui::MainWindow *ui;
    MH_Memory _MHManager;
    json Settings;
    std::vector<QLineEdit*> _InputBoxes;
    bool _SafeMode = true;
private slots:
    void _FindAddr();
    void _FetchData(bool noMessage = false);
    void _WriteData();
    void _aboutInfo();
};

class AboutWindow : public QDialog
{
    Q_OBJECT
  public:
    explicit AboutWindow(QWidget *parent = 0);
    ~AboutWindow();

  private:
    Ui::AboutWindow *ui;
};

class DialogMessage : public QDialog
{
    Q_OBJECT
  public:
    DialogMessage(QWidget *parent, const std::string &Title, const std::string &Message);

  private:
    QLabel _Message;
    QPushButton _okButton;
};
