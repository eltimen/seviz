#include "stwindow.h"
#include "ui_stwindow.h"

STWindow::STWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::STWindow)
{
    ui->setupUi(this);
}

STWindow::~STWindow()
{
    delete ui;
}
