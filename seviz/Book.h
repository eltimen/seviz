#pragma once
#include <QObject>
#include <QString>
#include <QStringList>
#include <QWebEngineView>
#include <QTemporaryDir>
#include "epubrenderer.h"
#include "ModuleManager.h"
#include "BookModels.h"

class Book : public QObject
{
    Q_OBJECT

public:
    Book(const QString& epubPath, EpubRenderer* view, ModuleManager& moduleManager);
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
    ModuleManager& m_moduleManager;

    int m_currentChapterIndex = -1;
    QList<Chapter> m_chapters;
};
