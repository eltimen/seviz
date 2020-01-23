#include "syntaxtreewindow.h"
#include "ui_syntaxtreewindow.h"

SyntaxTreeWindow::SyntaxTreeWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SyntaxTreeWindow)
{
    ui->setupUi(this);
}

SyntaxTreeWindow::~SyntaxTreeWindow()
{
    delete ui;
}
