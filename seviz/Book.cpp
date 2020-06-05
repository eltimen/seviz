#include "Book.h"
#include <QRegularExpression>
#include <QProgressDialog>
#include <JLCompress.h>
#include <QDir>
#include "exceptions.h"
#include "epubrenderer.h"
#include "ModuleManager.h"

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
    if (m_epubDir.isValid()) {
        QProgressDialog progress ("Открытие книги", "", 0, 0, m_moduleManager.mainWindow());
        progress.setCancelButton(nullptr);
        progress.show();
        QStringList files = JlCompress::extractDir(m_epubPath, m_epubDir.path());
        if (!files.empty()) {
            // TODO взять путь к opf из META_INF/container.xml. пока берем первый попавшийся opf из epub
            QString opf = files.filter(QRegularExpression(".\.opf$"))[0];
            Position::setBook(this);
            m_chapters = m_renderer->open(this, opf);
            m_moduleManager.bookOpened(this, m_epubDir, m_chapters);
        } else {
            throw InvalidEpubException();
        }
        progress.cancel(); // !!! не забыть убрать при возникновении исключения
    } else {
        throw IOException(m_epubDir.errorString());
    }
}

void Book::showChapter(int index) {
    m_currentChapterIndex = index;
    m_renderer->showChapter(index);

    if (m_chapters[index].sections.size() > 0) {
        m_moduleManager.triggerRerendering(m_chapters[index].firstPos(), m_chapters[index].lastPos());
    }
}

void Book::save() {
    QProgressDialog progress("Сохранение...", "", 0, 0, m_moduleManager.mainWindow());
    progress.setCancelButton(nullptr);
    progress.show();

    QDir dir(m_epubDir.path());
    dir.cd("seviz");
    
    m_moduleManager.forEachModule([this, &dir](AbstractModule* m) {
        if (dir.cd(m->id())) {
            m->save(dir);
            qDebug() << dir.path() + " save exist";
            dir.cdUp();
        } else if (dir.mkdir(m->id()) && dir.cd(m->id())) {
            m->save(dir);
            qDebug() << dir.path() + " save new folder";
            dir.cdUp();
        } else {
            throw IOException("не удалось создать директорию для " + m->id());
        }
    });

    QString tmpPath = m_epubPath + "_";
    if (!JlCompress::compressDir(tmpPath, m_epubDir.path()) ||
        !QFile::remove(m_epubPath) ||
        !QFile::rename(tmpPath, m_epubPath)) {
        throw IOException("не удалось сохранить epub");
    }

    progress.cancel();
}

QStringList Book::getChapterTitles() {
    QStringList ret;
    for (const auto& c : m_chapters) {
        ret << c.name();
    }
    return ret;
}

const QList<Chapter>& Book::chapters() const {
    return m_chapters;
}

const Chapter& Book::getCurrentChapter() const {
    if (m_currentChapterIndex == -1) {
        throw -1;
    }
    return m_chapters[m_currentChapterIndex];
}

const Section& Book::getSection(const Position& pos) const {
    if (pos.chapterIndex() < m_chapters.size()) {
        const Chapter& ch = m_chapters[pos.chapterIndex()];

        if (!ch.isInitialized()) {
            throw std::logic_error("chapter isn't initialized. user need open chapter first");
        }

        if (pos.sectionIndex() < ch.sections.size()) {                  
                return ch.sections[pos.sectionIndex()];
        } else {
            throw std::out_of_range("incorrect section id");
        }
    } else {
        throw std::out_of_range("invalid chapter id");
    }
}

const Paragraph& Book::getParagraph(const Position& pos) const {
    const Section& sect = getSection(pos);
    if (pos.paragraphIndex() < sect.size()) {
        return sect[pos.paragraphIndex()];
    } else {
        throw std::out_of_range("invalid paragraph id");
    }
}

const Sentence& Book::getSentence(const Position& pos) const {
    const Paragraph& par = getParagraph(pos);
    if (pos.sentenceIndex() < par.size()) {
        return par[pos.sentenceIndex()];
    } else {
        throw std::out_of_range("invalid sentence id");
    }
}

const Word& Book::getWord(const Position& pos) const {
    const Sentence& sent = getSentence(pos);
    if (pos.wordIndex() < sent.size()) {
        return sent[pos.wordIndex()];
    } else {
        throw std::out_of_range("invalid word id");
    }
}

void Book::setModelForChapter(int chapterIndex, const QList<Section>& data) {
    m_chapters[chapterIndex].sections = data;
}

void Book::setModelForParagraph(int chapterIndex, int paragraphIndex, const QList<Sentence>& data) {
    m_chapters[chapterIndex].sections[0][paragraphIndex] = Paragraph(paragraphIndex+1, data);
}
