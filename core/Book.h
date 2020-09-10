#pragma once
#include <QObject>
#include <QString>
#include <QStringList>
#ifdef SEVIZ_CORE
    #include <QWebEngineView>
#endif
#include <QTemporaryDir>
#include "BookModels.h"

class EpubRenderer;
class PluginManager;

class SEVIZSHARED_EXPORT Book : public QObject
{
    Q_OBJECT

public:
    Book(const QString& epubPath, EpubRenderer* view, PluginManager& moduleManager);
    ~Book();

    void open();
    void showChapter(int index);
    void save();
    QStringList getChapterTitles();
    
    const QList<Chapter>& chapters() const;
    const Chapter& getCurrentChapter() const;
    const Section& getSection(const Position& pos) const;
    const Paragraph& getParagraph(const Position& pos) const;
    const Sentence& getSentence(const Position& pos) const;
    const Word& getWord(const Position& pos) const;

    void setModelForChapter(int chapterIndex, const QList<Section>& data);
    void setModelForParagraph(int chapterIndex, int paragraphIndex, const QList<Sentence>& data);

private:
    QTemporaryDir m_epubDir;
    EpubRenderer* m_renderer;
    QString m_epubPath;
    PluginManager& m_moduleManager;

    int m_currentChapterIndex = -1;
    QList<Chapter> m_chapters;
};
