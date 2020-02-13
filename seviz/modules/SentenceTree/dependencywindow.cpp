#include "dependencywindow.h"
#include "ui_dependencywindow.h"

DependencyWindow::DependencyWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DependencyWindow)
{
    ui->setupUi(this);
    ui->webEngineView->setHtml("TODO");
}

DependencyWindow::~DependencyWindow()
{
    delete ui;
}
