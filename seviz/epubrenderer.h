#ifndef EPUBRENDERER_H
#define EPUBRENDERER_H

#include <QEventLoop>
#include <QObject>
#include <QWebEngineView>
#include <QVariant>
#include <QVector>
#include "BookModels.h"
#include "DomChapter.h"

class Book;

class EpubRenderer : public QObject
{
    Q_OBJECT
public:
    explicit EpubRenderer(QWebEngineView*);

    QList<Chapter> open(Book* book, const QString& opfPath);
    void showChapter(int index);
    void close();

    void updateChapterView(const DomChapter& dom);
    DomChapter generateDomChapter();

    // int addHandler(enum ElementType elem, enum EventType on, std::function<void()>& slot);
    void removeHandler(int i);
    QPair<Position, Position> selectedTextPos();
    QString selectedText();
   
public slots:
    // функции, доступные из js
    void setChaptersList(const QVariant&);
    void setModelDataForChapter(int chapterIndex, const QVariant& paragraphs);

private:
    QWebEngineView *m_view;
    QWebChannel* m_webchannel;
    QEventLoop m_loop;
    Book* m_book = nullptr;

    QList<Chapter> m_chapterTitles;
};

#endif // EPUBRENDERER_H
