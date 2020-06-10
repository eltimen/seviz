#include "frameelement.h"

#include <memory>
#include <QString>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include "framenet.h"
#include "framenetmodel.h"
#include "stwindow.h"

FrameElement::FrameElement(const QString& name, Frame* child)
    : FrameElement(name, std::shared_ptr<Frame>(child))
{ }

FrameElement::FrameElement(const QString& name, std::shared_ptr<Frame> child) 
    : m_subFrame(child),
    m_name(name)
{
    assert(child->range().first != -1);
}

FrameElement::FrameElement(const QString& name, const std::vector<Word>& phrase)
    : m_words(phrase),
    m_name(name) {
    assert(phrase.size() > 0);
    // TODO assert слова в фразе должны быть соседями
}

FrameElement::~FrameElement() {
}

bool FrameElement::isWord() const {
    return !m_subFrame;
}

bool FrameElement::isFrame() const {
    return (bool)m_subFrame;
}

bool FrameElement::isEmpty() const {
    return m_name.isEmpty();
}

const QString& FrameElement::name() const {
    return m_name;
}

WordRange FrameElement::range() const {
    if (m_subFrame) {
        return m_subFrame->range();
    } else {
        return WordRange(m_words.begin()->id(), (m_words.end() - 1)->id());
    }
}

std::shared_ptr<Frame> FrameElement::childFrame() const {
    return m_subFrame;
}

const std::vector<Word>& FrameElement::words() const {
    return m_words;
}

FrameElement FrameElement::fromJson(const QJsonValue& json, const FrameNetModel& frameNetDb, IEngine* engine, const Position& sentPos) {
    QJsonObject obj = json.toObject();

    QString name = obj.value("name").toString();
    QString type = obj.value("type").toString();

    if (type == "frame") {
        QString subFrameStr = QJsonDocument(obj.value("subFrame").toObject()).toJson();
        Frame* subFrame = Frame::fromJson(subFrameStr, frameNetDb, engine, sentPos);
        return FrameElement(name, subFrame);
    } else if (type == "words") {
        int fromWordId = obj.value("wordFromId").toInt();
        int toWordId = obj.value("wordToId").toInt();

        Position from(sentPos.chapterId(), sentPos.sectionId(), sentPos.paragraphId(), sentPos.sentenceId(), fromWordId);
        Position to(sentPos.chapterId(), sentPos.sectionId(), sentPos.paragraphId(), sentPos.sentenceId(), toWordId);

        std::vector<Word> words = STWindow::getWordsInsideFrameRange(engine, from, to);
        return FrameElement(name, words);
    } else {
        //throw;
    }


    return FrameElement("empty", {});
}

QString FrameElement::toJson() const {
    QString ret(R"(
    {
        "name": "%1", 
        "type" : "%2",
        "subFrame": %3,
        "wordFromId": %4,
        "wordToId": %5
    }
    )");

    ret = ret.arg(m_name)
             .arg(m_subFrame ? "frame" : "words")
             .arg(m_subFrame ? m_subFrame->toJson() : "{}")
             .arg(!m_words.empty() ? m_words.front().id() : -1)
             .arg(!m_words.empty() ? m_words.back().id() : -1);


    return ret;
}

void FrameElement::toTreantJson(QString& ret, int depth, int maxDepth, const QPair<QString, QString>& colors) const {
    if (isFrame()) {
        m_subFrame->toTreantJson(ret, depth, maxDepth, m_name, colors);
    } else if (!isEmpty()) { // TODO fix empty FEs bug, then remove "if" here
        int dropLevel = maxDepth - 1 - depth;

        for (int i = 0; i < dropLevel; ++i) {
            ret.append("{ pseudo: true, children: [");
        }

        QString constituent;
        for (const auto& w : m_words) {
            constituent += w.text() + " ";
        }

        ret += QStringLiteral(R"(
        {
        "text": { title: "%1", name: "%2" }, 
        "HTMLclass": "%3",
        "title_fgcolor": "%4",
        "title_bgcolor": "%5",
        "HTMLid": "%6",
        "children": [
        )") .arg(m_name)
            .arg(constituent)
            .arg("word")
            .arg(colors.second)
            .arg(colors.first)
            .arg(QString("fe-word-") + QString::number(m_words.begin()->id()));

        ret += "]} \n";

        for (int i = 0; i < dropLevel; ++i) {
            ret.append("]}");
        }
    }
    

   

    
}
