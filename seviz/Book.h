#pragma once

#include <QObject>
#include <QWebEngineView>
#include "epubrenderer.h"

class Book : public QObject
{
	Q_OBJECT

public:
	Book(QWebEngineView *view);
	~Book();

	// open - распаковать во временную папку и отрендерить рендером
	// save - забрать dom с рендера и упаковать содержимое временной папки в epub в epub
	// close - сохранить?, затем убрать из рендера
	// hasSavedChanges ?
	// reload

	// getExtraContent(key) // у каждого плагина свое хранилище 
	// setExtraContent(key, value);

	// getRenderer

private:
	EpubRenderer m_renderer;
};
