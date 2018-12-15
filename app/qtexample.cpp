#include "qtexample.h"
#include "ui_qtexample.h"

MHWMkArmor::MHWMkArmor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MHWMkArmor)
{
    ui->setupUi(this);
}

MHWMkArmor::~MHWMkArmor()
{
    delete ui;
}
