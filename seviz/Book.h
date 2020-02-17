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
    void showChapter(int chapterIndex);
    void save();
    QStringList getChapterTitles();
    
    Chapter& getCurrentChapter();
    Section& getSection(const Position& pos);
    Paragraph& getParagraph(const Position& pos);
    Sentence& getSentence(const Position& pos);
    Word& getWord(const Position& pos);

    void setModelForChapter(int chapterIndex, const QList<Section>& data);

private:
    void initModel();
    void prepareModuleDirectories();

    QTemporaryDir m_tempDir;
    EpubRenderer* m_renderer;
    QString m_epubPath;
    ModuleManager& m_moduleManager;

    int m_currentChapterIndex = -1;
    QVector<Chapter> m_chapters;
};
