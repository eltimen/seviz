#include "Book.h"
#include <QRegularExpression>
#include <JLCompress.h>
#include "exceptions.h"

#pragma warning(disable:4129)

Book::Book(const QString& epubPath, EpubRenderer* view, ModuleManager& moduleManager) :
    QObject(nullptr),
    m_renderer(view),
    m_epubPath(epubPath),
    m_moduleManager(moduleManager)
{
}

Book::~Book()
{
}

void Book::open() {
    // распаковка во временную папку
    if (m_tempDir.isValid()) {
        QStringList files = JlCompress::extractDir(m_epubPath, m_tempDir.path());
        if (!files.empty()) {
            // TODO взять путь к opf из META_INF/container.xml. пока берем первый попавшийся opf из epub
            QString opf = files.filter(QRegularExpression(".\.opf$"))[0];
            m_renderer->open(opf);
        } else {
            throw InvalidEpubException();
        }

    } else {
        throw CantUnpackEpubException(m_tempDir.errorString());
    }
}