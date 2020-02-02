#ifndef EPUBRENDERER_H
#define EPUBRENDERER_H

#include <QObject>
#include <QWebEngineView>
#include <QVariant>
#include <QVector>
#include "BookModels.h"

class EpubRenderer : public QObject
{
    Q_OBJECT
public:
    explicit EpubRenderer(QWebEngineView*);

    // открывает книгу. Внимание: работает асинхронно!
    void open(const QString& opfPath);
    void showChapter(int index);
    void close();
    // взять изменения dom?
    // reload
    
    // [[deprecated("При использовании возможны конфликты с JS/CSS других плагинов.")]] ?
    // запустить js на странице
    // добавить css на страницу

    // геттеры элементов модели по их позиции
    // зарегистрировать обработчик
    // снять регистрацию обработчика
    // сигнал для сбора данных для показа элемента модели
    
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
