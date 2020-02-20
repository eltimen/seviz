#include "Book.h"
#include <QRegularExpression>
#include <JLCompress.h>
#include <QDir>
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
    if (m_epubDir.isValid()) {
        QStringList files = JlCompress::extractDir(m_epubPath, m_epubDir.path());
        if (!files.empty()) {
            // TODO взять путь к opf из META_INF/container.xml. пока берем первый попавшийся opf из epub
            QString opf = files.filter(QRegularExpression(".\.opf$"))[0];
            m_chapters = m_renderer->open(this, opf);
            m_moduleManager.bookOpened(this);
            prepareModuleDirectoriesAndLoad();
        } else {
            throw InvalidEpubException();
        }

    } else {
        throw IOException(m_epubDir.errorString());
    }
}

void Book::showChapter(int index) {
    m_currentChapterIndex = index;
    m_renderer->showChapter(index);
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

void Book::prepareModuleDirectoriesAndLoad() {
    m_moduleManager.forEachModule([this](AbstractModule* m) {
        QDir dir(m_epubDir.path());
        dir.mkdir("seviz");
        dir.cd("seviz");
        if (dir.mkdir(m->id())) {
                // закомментировано: если папки не было, то модулю грузить нечего
                dir.cd(m->id());
                // m->load(dir);
                qDebug() << dir.path() + " created";
        } else {
            // проверка, что такая папка есть, а не произошла ошибка при создании
            if (dir.cd(m->id())) {
                m->load(dir); 
            } else {
                throw IOException("не удалось создать директорию для " + m->id());
            }
        }
    });
}
