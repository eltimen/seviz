#ifndef EPUBRENDERER_H
#define EPUBRENDERER_H

#include <QEventLoop>
#include <QObject>
#include <QFlags> 
#include <QWebEngineView>
#include <QVariant>
#include <QVector>
#include "BookModels.h"
#include "EventModels.h"
#include "DomChapter.h"

class Book;

class EpubRenderer : public QObject
{
    Q_OBJECT
public:
    explicit EpubRenderer();
    void setWidget(QWebEngineView* widget);

    QList<Chapter> open(Book* book, const QString& opfPath);
    void showChapter(int index);
    void close();

    void tokenizeChapter(int index);
    QString getParagraphText(int chapterIndex, int paragraphIndex); // реализовать частичное перетокенизирование (отдельных абзацев)?
    void setParagraphText(int chapterIndex, int paragraphIndex);

    void updateChapterView(const DomChapter& dom);

    void addHandler(const Handler& h);
    void removeHandler(const Handler& h);

    QPair<Position, Position> selectedTextPos();
    Position mouseHoverElement();
   
public slots:
    // функции, доступные из js
    void setChaptersList(const QVariant&);
    void setModelDataForChapter(int chapterIndex, const QVariant& paragraphs);
    void processEvent(const QByteArray& mouseEvent);

private:
    QWebEngineView *m_view;
    QWebChannel* m_webchannel;
    QEventLoop m_openLoop;
    QEventLoop m_tokenizeLoop;
    Book* m_book = nullptr;

    QMultiHash<QString, Handler> m_handlers;

    QList<Chapter> m_chapterTitles;

    QString eventToString(EventType);
};

#endif // EPUBRENDERER_H
