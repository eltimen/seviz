#include "frame.h"
#include <QSet>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <cassert>
#include "stwindow.h"

Frame::Frame(const QString& name, const Word& lu, const WordRange& range, const std::vector<Word>& words, const FrameNetModel& frameNetDb)
    : m_name(name),
    m_lu(lu),
    m_words(words.begin(), words.end()),
    m_currentWords(words.begin(), words.end()),
    m_allowedElements(frameNetDb.frameElementsFor(name)),
    m_range(range),
    m_frameNetDb(frameNetDb)
{
    // для упрощения вывода дерева LU заносится как FE "LU"
    assert(WordRange(lu.id(), lu.id()) == FrameElement("LU", { lu }).range());
    m_elements.emplace(WordRange(lu.id(), lu.id()), FrameElement("LU", { lu }));
}

Frame::~Frame() {
    qDebug() << m_name << " is dead";
}

QString Frame::name() const {
    return m_name;
}

const Word& Frame::lu() const {
    return m_lu;
}

std::vector<Word> Frame::words() const {
    return std::vector<Word>(m_currentWords.begin(), m_currentWords.end());
}

WordRange Frame::range() const {
    return m_range;
}

std::vector<std::shared_ptr<Frame>> Frame::subFrames() const {
    std::vector<std::shared_ptr<Frame>> ret;
    for (const auto& rangeAndFE : m_elements) {
        if (rangeAndFE.second.isFrame()) {
            ret.push_back(rangeAndFE.second.childFrame());
        }
    }
    return ret;
}

const std::map<WordRange, FrameElement>& Frame::elements() const {
    return m_elements;
}

QStringList Frame::elementsList() const {
    return m_allowedElements;
}

QStringList Frame::getFreeElementsList() const {
    QSet<QString> fes;
    for (const auto& fe : m_elements) {
        fes << fe.second.name();
    }
    return QStringList::fromSet(m_allowedElements.toSet() - fes);
}

FrameElement& Frame::operator[](const QString& feName) {
    return m_elements.at(m_rangeByElementName.at(feName));
}

void Frame::clearElements() {
    // std::remove_if not works with map
    for (auto iter = m_elements.begin(); iter != m_elements.end(); ) {
        if (iter->second.name() != "LU")
            iter = m_elements.erase(iter);
        else
            ++iter;
    }
    m_rangeByElementName.clear();
    m_currentWords = m_words;
}

Frame* Frame::fromJson(const QString& json, const FrameNetModel& frameNetDb, IEngine* engine, const Position& sentPos) {
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8(), &error);
    if (!doc.isNull()) {
        if (json == "{}") { // TODO replace
            return nullptr;
        }

        QString name = doc.object().value("name").toString();
        int fromWordId = doc.object().value("from").toInt();
        int toWordId = doc.object().value("to").toInt();
        int luId = doc.object().value("luId").toInt();
        int treeId = doc.object().value("treeId").toInt();

        Position from(sentPos.chapterId(), sentPos.sectionId(), sentPos.paragraphId(), sentPos.sentenceId(), fromWordId);
        Position to(sentPos.chapterId(), sentPos.sectionId(), sentPos.paragraphId(), sentPos.sentenceId(), toWordId);
        std::vector<Word> frameWords = STWindow::getWordsInsideFrameRange(engine, from, to);
        WordRange range(frameWords[0].id(), frameWords[frameWords.size() - 1].id());

        Word lu = *std::find_if(frameWords.begin(), frameWords.end(), [luId](const Word& w) {return w.id() == luId; });
        assert(lu.id() == luId);

        Frame* frame = new Frame(name, lu, range, frameWords, frameNetDb);
        frame->setTreeId(treeId);

        for (const QJsonValue& elem : doc.object().value("elements").toArray()) {
            FrameElement fe = FrameElement::fromJson(elem, frameNetDb, engine, sentPos);
            if (fe.name() != "LU") {
                frame->setElement(fe);
            }
        }
        return frame;
    } else {
        throw QString("framenet tree ") + error.errorString();
    }

    return nullptr; 
}

QString Frame::toJson() const {
    QString ret(R"(
    {
        "name": "%1", 
        "from" : %2,
        "to" : %3,
        "luId" : %4,
        "treeId" : %5,
        "elements": [
    )");

    ret = ret.arg(m_name)
             .arg(m_range.first)
             .arg(m_range.second)
             .arg(m_lu.id())
             .arg(m_treeId);

    bool first = true;
    for (const auto& fe : m_elements) {
        if (!first) ret += ",";
        ret += fe.second.toJson();
        first = false;
    }

    ret += "]}";

    return ret;
}

void Frame::setElement(const FrameElement& val) {
    assert(m_allowedElements.contains(val.name()));
    assert(!val.range().contains(m_lu.id()));

    if (val.isFrame()) {
        for (auto it = m_currentWords.begin(); it != m_currentWords.end(); ) {
            if (val.childFrame()->range().contains(it->id())) {
                it = m_currentWords.erase(it);
            } else {
                ++it;
            }
        }
    }
    
    m_elements.emplace(val.range(), val);
    m_rangeByElementName.emplace(val.name(), val.range());
}

FrameElement Frame::takeElement(const QString& name) {
    WordRange range = m_rangeByElementName.at(name);
    FrameElement ret = m_elements.at(range);
    m_rangeByElementName.erase(name);
    m_elements.erase(range);
    return ret;
}

int Frame::treeId() const {
    return m_treeId;
}

void Frame::setTreeId(int id) {
    m_treeId = id;
}

void Frame::toTreantJson(QString& ret, int depth, int maxDepth, const QString& parentFe, const QPair<QString, QString>& parentFEcolors) const {
    ret += QStringLiteral(R"(
        {
        "text": { title: "%1", name: "%2" }, 
        "HTMLclass": "frame",
        "HTMLid": "%3",
        "title_fgcolor": "%4",
        "title_bgcolor": "%5",
        "children": [
    )").arg(parentFe)
        .arg(m_name)
        .arg(QString::number(m_treeId))
        .arg(parentFEcolors.second)
        .arg(parentFEcolors.first);

    
    std::map<WordRange, std::vector<Word>> freeWordRanges;
    int currentFrom = -1;
    int currentTo = -1;
    std::vector<Word> wordsInsideRange;
    for (const Word& w : m_words) {
        bool noFeWithWord = true;
        for (const std::pair<WordRange, FrameElement>& fe : m_elements) {
            if (fe.first.contains(w.id())) {
                noFeWithWord = false;
                break;
            }
        }
        if (wordsInsideRange.empty()) {
            currentFrom = w.id();
            currentTo = currentFrom;
        } else {
            if (noFeWithWord && w.id() != m_lu.id()) {
                ++currentTo;
            } else {
                freeWordRanges.emplace(WordRange(currentFrom, currentTo), wordsInsideRange);
                currentFrom = w.id();
                currentTo = currentFrom;
                wordsInsideRange.clear();
            } 
        }
        if (noFeWithWord && w.id() != m_lu.id())
            wordsInsideRange.emplace_back(w);
    }
    if (!wordsInsideRange.empty()) {
        freeWordRanges.emplace(WordRange(currentFrom, currentTo), wordsInsideRange);
    }

    for (const std::pair<WordRange, FrameElement>& fe : m_elements) {
        while (freeWordRanges.size() > 0 && freeWordRanges.begin()->first < fe.first) {
            FrameElement("???", freeWordRanges.begin()->second).toTreantJson(ret, depth + 1, maxDepth, qMakePair(QString("FFFFFF"), QString("000000")));
            ret += ",";
            freeWordRanges.erase(freeWordRanges.begin());
        }
        // если есть разрыв с предыдущим, то что между разрывом вставить как ???
        auto colors = m_frameNetDb.getColorsForFE(m_name, fe.second.name());
        fe.second.toTreantJson(ret, depth + 1, maxDepth, colors);
        ret += ",";
    }
    while (!freeWordRanges.empty()) {
        FrameElement("???", freeWordRanges.begin()->second).toTreantJson(ret, depth + 1, maxDepth, qMakePair(QString("FFFFFF"), QString("000000")));
        ret += ",";
        freeWordRanges.erase(freeWordRanges.begin());
    }

    ret += "]} \n";
}

void Frame::removeFeWhichContainSubframe(int nodeId) {
    for (decltype(m_elements)::iterator it = m_elements.begin(); it != m_elements.end(); ++it) {
        std::shared_ptr<Frame> child = it->second.childFrame();
        if (child) {
            if (child->m_treeId == nodeId) {
                m_currentWords.insert(child->m_words.begin(), child->m_words.end());
                m_rangeByElementName.erase(it->second.name());
                it = m_elements.erase(it);
                break;
            } else {
                child->removeFeWhichContainSubframe(nodeId);
            }
        }
    }
    assert(m_elements.size()-1 == m_rangeByElementName.size());
}

Frame* Frame::find(int id) {
    if (this->m_treeId == id) {
        return this;
    }
    Frame* found;
    for (const auto& fe : m_elements) {
        if (fe.second.isFrame()) {
            std::shared_ptr<Frame> child = fe.second.childFrame();
            if (found = child->find(id)) {
                return found;
            }
        }
    }
    return nullptr;
}

Frame* Frame::findLeastParentForRange(const WordRange& range) const {
    const Frame* found = this;
    for (const std::pair<WordRange, FrameElement>& fe : m_elements) {
        if (fe.second.isFrame() && fe.first.isOutsideOf(range)) {
            found = fe.second.childFrame()->findLeastParentForRange(range);
            break;
        }
    }
    return (Frame*)found;
}

int Frame::maxDepth() const {
    int depth = 0;
    for (const std::pair<WordRange, FrameElement>& fe : m_elements) {
        if (fe.second.isFrame()) {
            int d = fe.second.childFrame()->maxDepth();
            depth = std::max(depth, d);
        } else {
            depth = std::max(depth, 1);
        }
    }
    return depth + 1;
}

