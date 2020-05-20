#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Book.h"
#include "ModuleManager.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onFileOpen();
    void onFileSave();
    void onAbout();

    void onChapterChanged(int index);

    void disableConfictFeatures();
private:
    void setupModules();

    Ui::MainWindow *ui;
    Book* m_book = nullptr;
    EpubRenderer* m_bookViewer = nullptr;
    ModuleManager m_manager;

    QMap<Feature, QPair<QAction*,QAction*>> m_actions;
};

#endif // MAINWINDOW_H
