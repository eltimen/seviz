#include "epubrenderer.h"

#include <QWebChannel>
#include <QWebEngineSettings>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QMap>
#include "Book.h"
#include "exceptions.h"

EpubRenderer::EpubRenderer() :
    QObject(nullptr), //parent 
    m_view(nullptr),
    m_webchannel(new QWebChannel(this))
{
    // нужно вызвать setupUi перед передачей виджета, поэтому инициализация m_view через метод setWidget
}

void EpubRenderer::setWidget(QWebEngineView* widget) {
    m_view = widget;

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
    m_openLoop.exec();
    return std::move(m_chapterTitles);
}

void EpubRenderer::showChapter(int index) {
    QEventLoop loop;
    QString cmd = QStringLiteral(R"(window.render.display(%1))").arg(index);
    m_view->page()->runJavaScript(cmd, [&loop](const QVariant&) { loop.exit(0); });
    loop.exec();
}

void EpubRenderer::close() {
    QString cmd = QStringLiteral(R"(closeBook())");
    m_view->page()->runJavaScript(cmd);
}

QString EpubRenderer::serializeTokenizedChapters() const {
    QEventLoop loop;
    QString ret;

    QString cmd = QStringLiteral("window.render.serializeTokenizedChapters();");
    m_view->page()->runJavaScript(cmd, [&](const QVariant& json) {
        QJsonObject parsed = QJsonObject::fromVariantMap(json.toMap());
        ret = QJsonDocument(parsed).toJson(QJsonDocument::Compact);
        loop.exit(0);
    });
    loop.exec();

    return ret;
}

void EpubRenderer::deserializeTokenizedChapters(const QString& json) const {
    QEventLoop loop;
    QString cmd = QStringLiteral("window.render.deserializeTokenizedChapters(%1);").arg(json);
    m_view->page()->runJavaScript(cmd, [&](const QVariant&) { loop.exit(0); });
    loop.exec();
}

void EpubRenderer::tokenizeChapter(int index) {
    QString cmd = QStringLiteral(R"(window.render.tokenizeChapter(%1))").arg(index);
    m_view->page()->runJavaScript(cmd);
    m_tokenizeLoop.exec();
}

QString EpubRenderer::getParagraphText(const Position& pos) {
    QEventLoop loop;
    QString ret;
    QString cmd = QStringLiteral("render.chapterData[%1].querySelector(\"p:nth-of-type(%2)\").innerText;").arg(pos.chapterIndex()).arg(pos.paragraphId());
    m_view->page()->runJavaScript(cmd, [&](const QVariant& str) { 
        ret = str.toString();
        loop.exit(0); 
    });
    loop.exec();
    return ret;
}

void EpubRenderer::setParagraphText(const Position& pos, const QString& str) {
    QString cmd = QStringLiteral("replaceParagraphContent(%1, %2, \"%3\");").arg(pos.chapterId()).arg(pos.paragraphId()).arg(str);
    m_view->page()->runJavaScript(cmd, [&](const QVariant&) { });
}

void EpubRenderer::updateChapterView(const DomChapter& dom) {
    m_view->page()->runJavaScript("cleanupBeforeRender(window.render.viewer);");
    
    const QMap<Position, QString>& styles = dom.getStyles();
    for (QMap<Position, QString>::const_iterator it = styles.begin(); it != styles.end(); ++it) {
        QString cmd = QStringLiteral(R"(document.querySelector("%1").style.cssText = "%2";)").arg(it.key().cssSelector(), it.value());
        m_view->page()->runJavaScript(cmd);
    }

    const QMap<Position, QString>& tails = dom.getTails();
    for (QMap<Position, QString>::const_iterator it = tails.begin(); it != tails.end(); ++it) {
        QString cmd = QStringLiteral(R"(document.querySelector("%1").textContent = "%2";)").arg(it.key().cssSelector(), it.value());
        m_view->page()->runJavaScript(cmd);
    }
 
    const QMap<Position, QString>& tooltips = dom.getTooltips();
    for (QMap<Position, QString>::const_iterator it = tooltips.begin(); it != tooltips.end(); ++it) {
        QString cmd = QStringLiteral(R"(addTooltip("%1", "%2");)").arg(it.key().cssSelector(), it.value());
        m_view->page()->runJavaScript(cmd);
    }
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

QPair<Position, Position> EpubRenderer::selectedTextPos() {
    QPair<Position, Position> sel;
    bool selIsEmpty = true;
    QEventLoop loop;

    m_view->page()->runJavaScript(R"(getSelectionBorders())", [&](const QVariant& pair) {
        QVariantList list = pair.toList();
        if (!list.isEmpty()) {
            QVariantMap first = list[0].toMap();
            QVariantMap second = list[1].toMap();
            try {
                Position firstPos(m_book->getCurrentChapter().id(), 1, first["paragraph"].toInt(), first["sentence"].toInt(), first["word"].toInt());
                Position secondPos(m_book->getCurrentChapter().id(), 1, second["paragraph"].toInt(), second["sentence"].toInt(), second["word"].toInt());
                sel = qMakePair(firstPos, secondPos);
                selIsEmpty = false;
            } catch (std::invalid_argument&) {
                selIsEmpty = true;
            }  
        } 
        loop.exit(0);
    });
    loop.exec();

    if (selIsEmpty) {
        throw EmptySelectionException();
    }

    return sel;
}

Position EpubRenderer::mouseHoverElement() {
    Position ret;
    QEventLoop loop;

    m_view->page()->runJavaScript(R"(mouseHoverElement())", [&](const QVariant& pos) {
        QVariantMap map = pos.toMap();
        if (!map.isEmpty()) {
            ret = Position(m_book->getCurrentChapter().id(), 1, map["paragraph"].toInt(), map["sentence"].toInt(), map["word"].toInt());
        }
        loop.exit(0);
    });
    loop.exec();

    return ret;
}

void EpubRenderer::setChaptersList(const QVariant& objects) {
    // QVariantList<QVariantMap>
    //qDebug() << objects.toList().at(0).toMap()["href"].toString();
    m_chapterTitles.clear();
    int i = 1;
    for (const QVariant& toc : objects.toList()) {
        QVariantMap obj = toc.toMap();
        Chapter ch(m_book, i, obj.value("label").toString().trimmed());
        m_chapterTitles <<ch;
        ++i;
    }
    m_openLoop.exit(0);   
}

void EpubRenderer::setModelDataForParagraph(int chapterIndex, int parIndex, const QVariant& par) {
    QList<Sentence> sentences;
    for (const QVariant& s : par.toMap().value("sentences").toList()) {
        QList<Word> words;
        for (const QVariant& w : s.toMap().value("words").toList()) {
            words.push_back(Word(w.toMap()["id"].toInt(), w.toMap()["text"].toString()));
        }
        sentences.push_back(Sentence(s.toMap()["id"].toInt(), words));
    }
    m_book->setModelForParagraph(chapterIndex, parIndex, sentences);
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
    m_book->setModelForChapter(chapterIndex, QList<Section> {Section(1, paragraphs)});
    m_tokenizeLoop.exit(0); // TODO
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
        int id = el.toObject()["id"].toString().toInt();
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

    try {
        Position pos(m_book->getCurrentChapter().id(), 1, parId, sentId, wordId);
        QString type = eventData["type"].toString();
        bool alt = eventData["altKey"].toBool();
        bool shift = eventData["shiftKey"].toBool();
        bool ctrl = eventData["ctrlKey"].toBool();

        for (const auto& h : m_handlers.values(type)) {
            if ((alt == (h.modifierKey == ALT)) &&
                (shift == (h.modifierKey == SHIFT)) &&
                (ctrl == (h.modifierKey == CTRL)) &&
                pos.hasElement(h.onElements)) {
                h.slot(pos);
            }
        }
    } catch (int) {

    }
}

QString EpubRenderer::eventToString(EventType e) {
    switch (e) {
    case EventType::MOUSE_LCLICK:   return "click";
    case EventType::MOUSE_RCLICK:   return "contextmenu"; 
    case EventType::MOUSE_DBLCLICK: return "dblclick"; 
    case EventType::MOUSE_AUXCLICK: return "auxclick";
    case EventType::MOUSE_UP:       return "mouseup";
    case EventType::MOUSE_DOWN:     return "mousedown";
    case EventType::MOUSE_MOVE:     return "mousemove";
    case EventType::MOUSE_OVER:     return "mouseover";
    case EventType::MOUSE_OUT:      return "mouseout";
    default:
        assert(false);
        return "error!";
    };
}

