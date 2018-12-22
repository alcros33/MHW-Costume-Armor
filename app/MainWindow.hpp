#pragma once
#include <QMainWindow>

#include "ui_MainWindow.h"
#include "ui_AboutWindow.h"
#include "ui_DialogWindow.h"

#include "MHMemory.hpp"
#include "json.hpp"

// for convenience
using json = nlohmann::json;

namespace Status
{
    const int SUCCESS = 0;
    const int WARNING = 1;
    const int ERROR0 = 2;
    const std::vector<std::string> Names{"success","warning","error"};
} // namespace Status

namespace Ui {
class MainWindow;
class AboutWindow;
class DialogWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void debugPrints() const ;
    void show();

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

class DialogWindow : public QDialog
{
    Q_OBJECT
  public:
    DialogWindow(QWidget *parent, const std::string &Title, const std::string &Message, int Status);
    ~DialogWindow();

  private:
    Ui::DialogWindow *ui;
};
