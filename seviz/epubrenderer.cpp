#include "epubrenderer.h"
#include "Book.h"

#include <QWebChannel>
#include <QWebEngineSettings>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

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

void EpubRenderer::addHandler(const Handler& h) {
    // TODO hover (блокировать ли повторные события?)
    // TODO разобраться с конфликтами между видами элементов
    m_handlers.insert(eventToString(h.onEvent), h);
}

void EpubRenderer::removeHandler(const Handler& h) {
    QString key = eventToString(h.onEvent);
    for (const auto& i : m_handlers.values(key)) {
        if (i == h) {
            m_handlers.remove(key, i);
        }
    }
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
    QJsonDocument eventData = QJsonDocument::fromJson(mouseEvent);
    // {"altKey":false,"ctrlKey":false,"path":[{"id":"1","tagName":"SENTENCE"},{"id":"2","tagName":"P"},{"id":"viewer","tagName":"DIV"},{"id":"","tagName":"BODY"},{"id":"","tagName":"HTML"},{},{}],"shiftKey":false,"type":"mouseover"}
   
    QJsonArray path = eventData["path"].toArray();
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
    QString type = eventData["type"].toString();
    bool alt = eventData["altKey"].toBool();
    bool shift = eventData["shiftKey"].toBool();
    bool ctrl = eventData["ctrlKey"].toBool();
    
    for (const auto& h : m_handlers.values(type)) {
        if ((alt && h.modifierKey == ALT) && (shift && h.modifierKey == SHIFT) && (ctrl && h.modifierKey == CTRL) &&
            pos.hasElement(h.onElements)) {
            h.slot(pos);
        }

    }
}

QString EpubRenderer::eventToString(EventType e) {
    switch (e) {
    case EventType::MOUSE_LCLICK:   return "click";
    case EventType::MOUSE_RCLICK:   return "contextmenu"; 
    case EventType::MOUSE_DBLCLICK: return "dblclick"; 
    default:
        return "error!";
    };
}

