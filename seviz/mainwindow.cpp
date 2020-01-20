#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QToolbar>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_book(nullptr)
{
    ui->setupUi(this);

    m_book = new Book("ivanhoe.epub", ui->webEngineView);
    
    initToolbar();
}

MainWindow::~MainWindow()
{
    delete m_book;
    delete ui;
}

void MainWindow::initToolbar() {
    QToolBar* toolbar = ui->mainToolBar;
    m_manager.forEachModule([&](AbstractModule* module) {
        for (const Feature& f : module->getFeatures()) {
            QAction* action = toolbar->addAction(f.m_icon, f.m_text);
            action->setCheckable(true);
        }
    });
}
