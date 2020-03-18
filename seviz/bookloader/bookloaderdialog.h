#ifndef BOOKLOADERDIALOG_H
#define BOOKLOADERDIALOG_H

#include <QDialog>

namespace Ui {
class BookLoaderDialog;
}

class BookLoaderDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BookLoaderDialog(QWidget *parent = nullptr);
    ~BookLoaderDialog();

private:
    Ui::BookLoaderDialog *ui;
};

#endif // BOOKLOADERDIALOG_H
