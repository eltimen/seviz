#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QToolbar>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    connect(ui->fileOpenAction, &QAction::triggered, this, &MainWindow::onFileOpen);
    connect(ui->fileSaveAction, &QAction::triggered, this, &MainWindow::onFileSave);
    connect(ui->aboutAction, &QAction::triggered, this, &MainWindow::onAbout);

    m_bookViewer = new EpubRenderer(ui->webEngineView);

    initToolbar();
}

MainWindow::~MainWindow() {
    delete m_book;
    delete m_bookViewer;
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

void MainWindow::onFileOpen() {
    QString path = QFileDialog::getOpenFileName(this, "Открыть книгу", ".", "Файл EPUB (*.epub)");
    if (!path.isEmpty()) {
        delete m_book;
        m_book = new Book(path, m_bookViewer);
        m_book->open();
    }
}

void MainWindow::onFileSave() {

}

void MainWindow::onAbout() {

}