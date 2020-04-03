#include "bookloader.h"

BookLoader::BookLoader(ModuleManager* engine)
	: AbstractModule(engine, "_bookloader"),
	  m_engine(engine) {}

BookLoader::~BookLoader() {
	delete m_dialog;
}

QList<Feature> BookLoader::features() {
	return {};
}

bool BookLoader::importBook(QList<Chapter>& chapters, QDir& dir, QWidget* mainWindow) {
	delete m_dialog;
	m_dialog = new BookLoaderDialog(mainWindow, m_engine, chapters, dir);
    return m_dialog->exec();
}
