#include "epubrenderer.h"

#include <QWebChannel>
#include <QWebEngineSettings>

EpubRenderer::EpubRenderer(QWebEngineView* view) :
	QObject(nullptr), //parent 
	m_view(view),
	m_webchannel(new QWebChannel(this)) {

	m_view->page()->setWebChannel(m_webchannel);
	m_view->page()->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);

	m_view->setUrl(QUrl("file:///embedded_web_resources/index.html"));
}
