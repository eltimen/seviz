#include "epubrenderer.h"

#include <QWebChannel>
#include <QWebEngineSettings>

EpubRenderer::EpubRenderer(QWebEngineView* view) :
    QObject(nullptr), //parent 
    m_view(view),
    m_webchannel(new QWebChannel(this))
{
    m_webchannel->registerObject(QStringLiteral("core"), this);
    m_view->setContextMenuPolicy(Qt::NoContextMenu);
    m_view->page()->setWebChannel(m_webchannel);
    m_view->setUrl(QUrl("file:///embedded_web_resources/index.html"));
}

QVector<Chapter> EpubRenderer::open(const QString& opfPath) {
    // TODO добавить обработку ошибок
    close();
    QString cmd = QStringLiteral(R"(window.render.open("%1"))").arg(opfPath);
    m_view->page()->runJavaScript(cmd);
    m_loop.exec();
    return std::move(m_chapterTitles);
}

void EpubRenderer::showChapter(int index) {
    QString cmd = QStringLiteral(R"(window.render.display(%1))").arg(index);
    m_view->page()->runJavaScript(cmd);
}

void EpubRenderer::close() {
    QString cmd = QStringLiteral(R"(window.render.close())");
    m_view->page()->runJavaScript(cmd);
}

void EpubRenderer::setChaptersList(const QVariant& objects) {
    // QVariantList<QVariantMap>
    //qDebug() << objects.toList().at(0).toMap()["href"].toString();

    int i = 1;
    for (const QVariant& toc : objects.toList()) {
        QVariantMap obj = toc.toMap();
        Chapter ch(i, obj.value("label").toString().trimmed());
        m_chapterTitles <<ch;
    }
    m_loop.exit(0);   
}
