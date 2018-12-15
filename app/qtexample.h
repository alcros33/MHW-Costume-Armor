#pragma once
#include <QMainWindow>

namespace Ui {
class MHWMkArmor;
}

class MHWMkArmor : public QMainWindow
{
    Q_OBJECT

public:
    explicit MHWMkArmor(QWidget *parent = 0);
    ~MHWMkArmor();

private:
    Ui::MHWMkArmor *ui;
};

