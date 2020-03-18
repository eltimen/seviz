#include "bookloaderdialog.h"
#include "ui_bookloaderdialog.h"

BookLoaderDialog::BookLoaderDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BookLoaderDialog)
{
    ui->setupUi(this);

    connect(ui->okButton, &QPushButton::clicked, this, &BookLoaderDialog::accept);
    connect(ui->cancelButton, &QPushButton::clicked, this, &BookLoaderDialog::reject);
}

BookLoaderDialog::~BookLoaderDialog() {
    delete ui;
}
