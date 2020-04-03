#include "bookloaderdialog.h"
#include "ModuleManager.h"
#include "Book.h"
#include "ui_bookloaderdialog.h"

BookLoaderDialog::BookLoaderDialog(QWidget* parent, ModuleManager* engine, QList<Chapter>& chapters, QDir& dir)
	: QDialog(parent),
	  ui(new Ui::BookLoaderDialog),
	  m_engine(engine),
	  m_chapters(chapters),
	  m_dir(dir) {
	ui->setupUi(this);

	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	ui->chaptersTableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
	ui->chaptersTableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);

	connect(ui->okButton, &QPushButton::clicked, this, &BookLoaderDialog::accept);
	connect(ui->cancelButton, &QPushButton::clicked, this, &BookLoaderDialog::reject);
}

BookLoaderDialog::~BookLoaderDialog() {
    delete ui;
}

void BookLoaderDialog::showEvent(QShowEvent* event) {
	// emit ?
	ui->chaptersTableWidget->setRowCount(m_chapters.size());
	for (Chapter& ch : m_chapters) {
		ui->chaptersTableWidget->setItem(ch.id() - 1, 0, new QTableWidgetItem(ch.name()));
		ui->chaptersTableWidget->setItem(ch.id() - 1, 1, new QTableWidgetItem("Ожидает"));
	}
	setFixedWidth(width());
}
