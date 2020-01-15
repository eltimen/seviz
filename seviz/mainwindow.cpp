#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
	m_book(nullptr)
{
    ui->setupUi(this);

    m_book = new Book("ivanhoe.epub", ui->webEngineView);
    
}

MainWindow::~MainWindow()
{
    delete ui;
	delete m_book;
}
