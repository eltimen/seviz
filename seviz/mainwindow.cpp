#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtGlobal>
#include <QToolbar>
#include <QFileDialog>
#include <QDockWidget>
#include <QComboBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_manager(*m_bookViewer)
{
    ui->setupUi(this);
    
    connect(ui->fileOpenAction, &QAction::triggered, this, &MainWindow::onFileOpen);
    connect(ui->fileSaveAction, &QAction::triggered, this, &MainWindow::onFileSave);
    connect(ui->aboutAction, &QAction::triggered, this, &MainWindow::onAbout);

    m_bookViewer = new EpubRenderer(ui->webEngineView);
    connect(ui->chapterComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onChapterChanged); 

    setupModules();
}

MainWindow::~MainWindow() {
    delete m_book;
    delete m_bookViewer;
    delete ui;
}

void MainWindow::setupModules() {
    m_manager.forEachModule([&](AbstractModule* module) {
        // для каждой функции плагина
        for (const Feature& f : module->features()) {
            // добавление dock-окна
            f.window->setParent(this);
            f.window->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);
            f.window->setWindowTitle(f.name);
            addDockWidget(Qt::BottomDockWidgetArea, f.window);
            f.window->hide();

            // добавление кнопки на тулбар
            QToolBar* toolbar = ui->mainToolBar;
            QAction* action = toolbar->addAction(f.icon, f.name, [this, f](bool checked) {
                if (checked) {
                    m_manager.featureEnabled(f);
                    f.window->show();
                } else {
                    f.window->hide();
                    m_manager.featureDisabled(f);
                }
            });
            action->setCheckable(true);
        }
    });
}

void MainWindow::onFileOpen() {
    QString path = QFileDialog::getOpenFileName(this, "Открыть книгу", ".", "Файл EPUB (*.epub)");
    if (!path.isEmpty()) {
        delete m_book;
        m_book = new Book(path, m_bookViewer, m_manager); 
        m_book->open();
        ui->mainToolBar->setEnabled(true);
        ui->fileSaveAction->setEnabled(true);
        ui->chapterComboBox->setEnabled(true);
        ui->chapterComboBox->clear();
        ui->chapterComboBox->addItems(m_book->getChapterTitles());
    }
}

void MainWindow::onFileSave() {
    m_book->save();
}

void MainWindow::onAbout() {

}

void MainWindow::onChapterChanged(int index) {
    if (index >= 0)
        m_book->showChapter(index);
}
