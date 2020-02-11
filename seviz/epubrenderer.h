#ifndef EPUBRENDERER_H
#define EPUBRENDERER_H

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

    // открывает книгу. Внимание: работает асинхронно!
    void open(const QString& opfPath);
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
    
signals:
    void bookLoaded(const QVector<Chapter> &chapters);

private:
    QWebEngineView *m_view;
    QWebChannel* m_webchannel;
};

#endif // EPUBRENDERER_H
