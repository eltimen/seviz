#include "epubrenderer.h"
#include "Book.h"

#include <QWebChannel>
#include <QWebEngineSettings>

EpubRenderer::EpubRenderer(QWebEngineView* view) :
    QObject(nullptr), //parent 
    m_view(view),
    m_webchannel(new QWebChannel(this))
{
    m_view->page()->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    m_view->page()->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
    m_webchannel->registerObject(QStringLiteral("core"), this);
    m_view->setContextMenuPolicy(Qt::NoContextMenu);
    m_view->page()->setWebChannel(m_webchannel);
    m_view->setUrl(QUrl("file:///embedded_web_resources/index.html"));
}

QList<Chapter> EpubRenderer::open(Book* book, const QString& opfPath) {
    // TODO добавить обработку ошибок
    m_book = book;
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
    m_chapterTitles.clear();
    int i = 1;
    for (const QVariant& toc : objects.toList()) {
        QVariantMap obj = toc.toMap();
        Chapter ch(i, obj.value("label").toString().trimmed());
        m_chapterTitles <<ch;
    }
    m_loop.exit(0);   
}

void EpubRenderer::setModelDataForChapter(int chapterIndex, const QVariant& data) {
    QVariantList pars = data.toList();
    QList<Paragraph> paragraphs;
    for (const QVariant& p : pars) {
        QList<Sentence> sentences;
        for (const QVariant& s : p.toMap().value("sentences").toList()) {
            QList<Word> words;
            for (const QVariant& w : s.toMap().value("words").toList()) {
                words.push_back(Word(w.toMap()["id"].toInt(), w.toMap()["text"].toString()));
            }
            sentences.push_back(Sentence(s.toMap()["id"].toInt(), words));
        }
        paragraphs.push_back(Paragraph(p.toMap()["id"].toInt(), sentences));
    }
    qDebug() << "model " << chapterIndex;
    m_book->setModelForChapter(chapterIndex, QList<Section> {Section(1, paragraphs)});
}

void EpubRenderer::processEvent(const QByteArray& mouseEvent) {
    QJsonDocument event = QJsonDocument::fromJson(mouseEvent);
    // {"altKey":false,"ctrlKey":false,"path":[{"id":"1","tagName":"SENTENCE"},{"id":"2","tagName":"P"},{"id":"viewer","tagName":"DIV"},{"id":"","tagName":"BODY"},{"id":"","tagName":"HTML"},{},{}],"shiftKey":false,"type":"mouseover"}
   
    QJsonArray path = event["path"].toArray();
    int wordId = -1;
    int sentId = -1;
    int parId = -1;
    for (const auto& el : path) {
        const QString& tag = el.toObject()["tagName"].toString();
        int id = el.toObject()["id"].toInt();
        if (tag == "WORD") {
            wordId = id;
        } else if (tag == "SENTENCE") {
            sentId = id;
        } else if (tag == "P") {
            parId = id;
        } else {
            break;
        }
    }

    Position pos(m_book->getCurrentChapter().id(), 1, parId, sentId, wordId);
    QString type = event["type"].toString();
    bool alt = event["altKey"].toBool();
    bool shift = event["shiftKey"].toBool();
    bool ctrl = event["ctrlKey"].toBool();
    qDebug() << alt;
}

