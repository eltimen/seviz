#include "constituencywindow.h"
#include "ui_constituencywindow.h"

ConstituencyWindow::ConstituencyWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConstituencyWindow)
{
    ui->setupUi(this);
}

ConstituencyWindow::~ConstituencyWindow()
{
    delete ui;
}
