#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
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

private:
    void initToolbar();

    Ui::MainWindow *ui;
    Book* m_book;
    ModuleManager m_manager;
};

#endif // MAINWINDOW_H
