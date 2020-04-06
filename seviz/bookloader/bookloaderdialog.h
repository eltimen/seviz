#ifndef BOOKLOADERDIALOG_H
#define BOOKLOADERDIALOG_H

#include <QDialog>
#include <QDir>
#include <QList>

class Chapter;
class BookLoader;
class ModuleManager;

namespace Ui {
class BookLoaderDialog;
}

class BookLoaderDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BookLoaderDialog(QWidget *parent, ModuleManager* engine, QList<Chapter>& chapters, QDir& dir);
    ~BookLoaderDialog();

    virtual void showEvent(QShowEvent* event) override;

private:
    Ui::BookLoaderDialog *ui;

    ModuleManager* m_engine;
    QList<Chapter>& m_chapters;
    QDir& m_dir;
};

#endif // BOOKLOADERDIALOG_H
