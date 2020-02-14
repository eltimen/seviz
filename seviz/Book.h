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
    void save();

    int getCurrentChapterNumber();

    Chapter& getChapter();
    Section& getSection(const Position& pos);
    Paragraph& getParagraph(const Position& pos);
    Sentence& getSentence(const Position& pos);
    Word& getWord(const Position& pos);

private:
    void initModel();
    void prepareModuleDirectories();

    QTemporaryDir m_tempDir;
    EpubRenderer* m_renderer;
    QString m_epubPath;
    ModuleManager& m_moduleManager;
};
