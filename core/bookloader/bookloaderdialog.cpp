#include "bookloaderdialog.h"

#include <QTimer>
#include "PluginManager.h"
#include "Book.h"
#include "ui_bookloaderdialog.h"

BookLoaderDialog::BookLoaderDialog(QWidget* parent, PluginManager* engine, QList<Chapter>& chapters, QDir& dir)
	: QDialog(parent, Qt::CustomizeWindowHint
					| Qt::WindowTitleHint
					| Qt::WindowSystemMenuHint
					| Qt::WindowMaximizeButtonHint),
	  ui(new Ui::BookLoaderDialog),
	  m_engine(engine),
	  m_chapters(chapters),
	  m_dir(dir) 
{
	ui->setupUi(this);

	ui->chaptersTableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
	ui->chaptersTableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);

	connect(ui->okButton, &QPushButton::clicked, this, &BookLoaderDialog::accept);
	connect(ui->cancelButton, &QPushButton::clicked, this, &BookLoaderDialog::reject);
}

BookLoaderDialog::~BookLoaderDialog() {
    delete ui;
}

void BookLoaderDialog::showEvent(QShowEvent* event) {
	Q_UNUSED(event);

	ui->chaptersTableWidget->setRowCount(m_chapters.size());
	for (Chapter& ch : m_chapters) {
		ui->chaptersTableWidget->setItem(ch.id() - 1, 0, new QTableWidgetItem(ch.name()));
		ui->chaptersTableWidget->setItem(ch.id() - 1, 1, new QTableWidgetItem("Ожидает"));
	}
	setFixedWidth(width());

	QTimer::singleShot(0, [this]() {
		ui->parsingProgressBar->setMinimum(0);
		ui->parsingProgressBar->setMaximum(m_chapters.size());
		for (Chapter& ch : m_chapters) {
			m_engine->getBookRender().tokenizeChapter(ch.id() - 1);
			ui->chaptersTableWidget->setItem(ch.id() - 1, 1, new QTableWidgetItem("Токенизировано"));
			ui->parsingProgressBar->setValue(ch.id());
		}
		ui->parsingProgressBar->setVisible(false);
		ui->okButton->setEnabled(true);

		//qDebug() << m_engine->getBookRender().getParagraphText(Position(4, 1, 1));
		//m_engine->getBookRender().setParagraphText(Position(4, 1, 1), "This is edited or fixed paragraph");
	});
}
