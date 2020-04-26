#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtGlobal>
#include <QToolbar>
#include <QFileDialog>
#include <QDockWidget>
#include <QComboBox>
#include <QMessageBox>
#include <QSignalBlocker>

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_bookViewer(new EpubRenderer()),
    m_manager(*m_bookViewer, this)
{
    ui->setupUi(this);
    m_bookViewer->setWidget(ui->webEngineView);
    ui->mainToolBar->setContextMenuPolicy(Qt::NoContextMenu);
    
    connect(ui->fileOpenAction, &QAction::triggered, this, &MainWindow::onFileOpen);
    connect(ui->fileSaveAction, &QAction::triggered, this, &MainWindow::onFileSave);
    connect(ui->aboutAction, &QAction::triggered, this, &MainWindow::onAbout);
    connect(ui->chapterComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onChapterChanged); 

    connect(ui->webEngineView, &QWebEngineView::loadFinished, [this](bool ok) {ui->fileMenu->setEnabled(ok); });

    setupModules();
}

MainWindow::~MainWindow() {
    delete m_book;
    delete m_bookViewer;
    try {
        delete ui;
    } catch (EmptySelectionException) {}
}

void MainWindow::setupModules() {
    m_manager.forEachModule([&](AbstractModule* module) {
        // для каждой функции плагина
        for (Feature& f : module->features()) {
            // добавление dock-окна
            f.window()->setParent(this);
            f.window()->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);
            f.window()->setWindowTitle(f.name());
            addDockWidget(f.dockLocation(), f.window());
            f.window()->hide();

            const auto onFeatureTriggered = [this, f](bool checked) mutable {
                m_actions[f].first->setChecked(checked);
                m_actions[f].second->setChecked(checked);

                if (checked) {
                    // TODO сделать неактивными все фичи, конфликтующие с включенной
                    m_manager.featureEnabled(f);
                    disableConfictFeatures();
                    f.window()->show();
                } else {
                    f.window()->hide();
                    disableConfictFeatures();
                    m_manager.featureDisabled(f);
                }
            };

            // добавление кнопки на тулбар
            QToolBar* toolbar = ui->mainToolBar;
            QAction* toolbarAction = toolbar->addAction(f.icon(), f.name(), onFeatureTriggered);
            toolbarAction->setCheckable(true);

            // добавление пунктов меню
            QMenu* menu = ui->modulesMenu;
            QAction* menuAction = menu->addAction(f.icon(), f.name(), onFeatureTriggered);
            if (f.menu()) {
                menuAction->setMenu(f.menu());
            }
            menuAction->setCheckable(true);

            m_actions.insert(f, qMakePair(toolbarAction,menuAction));
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
        ui->modulesMenu->setEnabled(true);
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

void MainWindow::disableConfictFeatures() {
    for (decltype(m_actions)::iterator it = m_actions.begin(); it != m_actions.end(); ++it) {
        it->first->setEnabled(true);
        it->second->setEnabled(true);
        it->first->setToolTip(it.key().name());
        it->second->setToolTip(it.key().name());
    }

    for (decltype(m_actions)::iterator it = m_actions.begin(); it != m_actions.end(); ++it) {
        // если эта feature активна 
        if (it->first->isChecked()) {
            // получаем для нее все конфликтные feature и блокируем их кнопки и меню
            QMultiMap<Feature, QString> conflicts = m_manager.getConflictFeaturesFor(it.key());
            for (Feature& f : conflicts.uniqueKeys()) {
                m_actions.value(f).first->setEnabled(false);
                m_actions.value(f).second->setEnabled(false);
                // TODO выводить почему конфликтует
                m_actions.value(f).first->setToolTip(QString("Функция отключена, т.к. конфликтует с функцией \"%1\"").arg(it.key().name()));
                m_actions.value(f).second->setToolTip(QString("Функция отключена, т.к. конфликтует с функцией \"%1\"").arg(it.key().name()));
            }
        }
    }
}
