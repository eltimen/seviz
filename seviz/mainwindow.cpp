#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

	ui->webEngineView->setUrl(QUrl("file:///embedded_web_resources/index.html"));
}

MainWindow::~MainWindow()
{
    delete ui;
}
