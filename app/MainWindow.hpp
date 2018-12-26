#pragma once
#include <QMainWindow>

#include "ui_MainWindow.h"
#include "AboutWindow.hpp"
#include "DialogWindow.hpp"

#include "MHMemory.hpp"
#include "json.hpp"

// for convenience
using json = nlohmann::json;

namespace Ui {
class MainWindow;
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
    void _NotImplemented();
};