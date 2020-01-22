#include "epubrenderer.h"

#include <QWebChannel>
#include <QWebEngineSettings>

EpubRenderer::EpubRenderer(QWebEngineView* view) :
    QObject(nullptr), //parent 
    m_view(view),
    m_webchannel(new QWebChannel(this))
{
    m_webchannel->registerObject(QStringLiteral("core"), this);
    m_view->page()->setWebChannel(m_webchannel);
    m_view->setUrl(QUrl("file:///embedded_web_resources/index.html"));
}

void EpubRenderer::open(const QString& opfPath) {
    close();
    QString cmd = QStringLiteral(R"(window.render.open("%1"))").arg(opfPath);
    qDebug() << cmd;
    m_view->page()->runJavaScript(cmd, [](const QVariant& v) { qDebug() << v.toString(); });
}

void EpubRenderer::close() {
    QString cmd = QStringLiteral(R"(window.render.close())");
    m_view->page()->runJavaScript(cmd, [](const QVariant& v) { qDebug() << v.toString(); });
}
