#include "bookloader.h"
#include <QFile>
#include <QTextStream>
#include "ModuleManager.h"

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

void BookLoader::load(QDir* dir) {
    assert(dir);

    // TODO реализовать прогресс открытия по загрузке модели и по каждому DOMу главы. скорее всего не здесь, а поближе к EpubRenderer

    // загрузка токенизированных глав из файла
    QFile file(dir->path() + "/tokenizedDom.json");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString str = in.readAll();
        m_engine->getBookRender().deserializeTokenizedChapters(str);
    } else {
        // TODO exception
    }
}

void BookLoader::save(QDir& dir) {
    // TODO реализовать прогресс-бар открытия по загрузке модели и по каждому DOMу главы. скорее всего не здесь, а поближе к EpubRenderer

    // сохранение токензированной главы в файл
    QFile file(dir.path() + "/tokenizedDom.json");
    // WARNING файл перезаписывается, даже если книга не была импортирована в этот раз
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << m_engine->getBookRender().serializeTokenizedChapters();
    } else {
        // TODO exception
    }
}
