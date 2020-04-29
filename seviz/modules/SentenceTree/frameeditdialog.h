#ifndef FRAMEEDITDIALOG_H
#define FRAMEEDITDIALOG_H

#include <QDialog>

namespace Ui {
class FrameEditDialog;
}

class FrameEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FrameEditDialog(QWidget *parent = nullptr);
    ~FrameEditDialog();

private:
    Ui::FrameEditDialog *ui;
};

#endif // FRAMEEDITDIALOG_H
