#include "framenetwindow.h"
#include "ui_framenetwindow.h"

FramenetWindow::FramenetWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FramenetWindow)
{
    ui->setupUi(this);
}

FramenetWindow::~FramenetWindow()
{
    delete ui;
}
