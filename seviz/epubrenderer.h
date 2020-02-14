#ifndef EPUBRENDERER_H
#define EPUBRENDERER_H

#include <QEventLoop>
#include <QObject>
#include <QWebEngineView>
#include <QVariant>
#include <QVector>
#include "BookModels.h"
#include "DomChapter.h"

class EpubRenderer : public QObject
{
    Q_OBJECT
public:
    explicit EpubRenderer(QWebEngineView*);

    QVector<Chapter> open(const QString& opfPath);
    void showChapter(int index);
    void close();

    void updateChapterView(const DomChapter& dom);
    DomChapter generateDomChapter();

    // int addHandler(enum ElementType elem, enum EventType on, std::function<void()>& slot);
    void removeHandler(int i);
    QPair<Position, Position> selectedTextPos();
    QString selectedText();
   
public slots:
    void setPageLoadedState(bool loaded) { 
        // conditional variable?
        qDebug() << "page loaded: " << loaded;
        //if (loaded)
            //this->open("ivanhoe.epub");
    }

    void setChaptersList(const QVariant&);

private:
    QWebEngineView *m_view;
    QWebChannel* m_webchannel;
    QEventLoop m_loop;

    QVector<Chapter> m_chapterTitles;
};

#endif // EPUBRENDERER_H
