#pragma once
#include <QObject>
#include <QWebEngineView>
#include "epubrenderer.h"

class Book : public QObject
{
    Q_OBJECT

public:
    Book(const QString& epubPath, EpubRenderer* view);
    ~Book();

    void open();
    bool hasUnsavedChanges();
    // open - распаковать во временную папку и отрендерить рендером
    // save - забрать dom с рендера и упаковать содержимое временной папки в epub в epub
    // close - сохранить?, затем убрать из рендера

    // getExtraContent(key) // у каждого плагина свое хранилище. нужно ли общее?
    // setExtraContent(key, value);

    // getRenderer

private:
    EpubRenderer* m_renderer;
    QString m_epubPath;
};
