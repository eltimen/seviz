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
    // TODO сделать функцию возвращающей массив глав (следовательно, синхронной)
    // из-за ограничений js нельзя подождать за промисов без await.
    // а async возвращает promise, который не дает передать управление в лямбду ниже и в ней сформировать chapters
    // возможное решение: таймер или conditional variable
    // TODO добавить обработку ошибок
    close();
    QString cmd = QStringLiteral(R"(window.render.open("%1"))").arg(opfPath);
    //qDebug() << cmd;
    m_view->page()->runJavaScript(cmd, [](const QVariant& v) { qDebug() << v.toString(); });
}

void EpubRenderer::showChapter(int index) {
    QString cmd = QStringLiteral(R"(window.render.display(%1))").arg(index);
    m_view->page()->runJavaScript(cmd, [](const QVariant& v) { qDebug() << v.toString(); });
}

void EpubRenderer::close() {
    QString cmd = QStringLiteral(R"(window.render.close())");
    m_view->page()->runJavaScript(cmd, [](const QVariant& v) { qDebug() << v.toString(); });
}

void EpubRenderer::setChaptersList(const QVariant& objects) {
    // QVariantList<QVariantMap>
    //qDebug() << objects.toList().at(0).toMap()["href"].toString();

    QVector<Chapter> chapters;
    int i = 1;
    for (const QVariant& toc : objects.toList()) {
        QVariantMap obj = toc.toMap();
        Chapter ch(i, obj.value("label").toString().trimmed());
        chapters << ch;
    }
    emit bookLoaded(chapters);
   
}
