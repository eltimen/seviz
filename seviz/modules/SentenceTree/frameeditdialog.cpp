#include "frameeditdialog.h"
#include "ui_frameeditdialog.h"

FrameEditDialog::FrameEditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FrameEditDialog)
{
    ui->setupUi(this);
}

FrameEditDialog::~FrameEditDialog()
{
    delete ui;
}
