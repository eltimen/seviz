#include "frame.h"
#include <QSet>
#include <QDebug>
#include <cassert>

Frame::Frame(const QString& name, const Word& lu, const WordRange& range, const std::vector<Word>& words, const FrameNetModel& frameNetDb)
    : m_name(name),
    m_lu(lu),
    m_words(words),
    m_currentWords(words),
    m_allowedElements(frameNetDb.frameElementsFor(name)),
    m_range(range) {
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

const std::vector<Word>& Frame::words() const {
    return m_currentWords;
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

void Frame::setElement(const FrameElement& val) {
    assert(m_allowedElements.contains(val.name()));
    assert(!val.range().contains(m_lu.id()));

    if (val.isFrame()) {
        m_currentWords.erase(std::remove_if(m_currentWords.begin(), m_currentWords.end(), [&val](const Word& w) {
            return val.childFrame()->range().contains(w.id());
        }), m_currentWords.end());
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

void Frame::toTreantJson(QString& ret, int depth, int maxDepth, const QString& parentFe) const {
    ret += QStringLiteral(R"(
        {
        "text": { title: "%1", name: "%2" }, 
        "HTMLclass": "frame",
        "HTMLid": "%3",
        "children": [
    )").arg(parentFe)
        .arg(m_name)
        .arg(QString::number(m_treeId));

    for (const std::pair<WordRange, FrameElement>& fe : m_elements) {
        fe.second.toTreantJson(ret, depth + 1, maxDepth);
        ret += ",";
    }

    ret += "]} \n";
}

void Frame::removeFEWithChild(int nodeId) {
    for (decltype(m_elements)::iterator it = m_elements.begin(); it != m_elements.end(); ++it) {
        std::shared_ptr<Frame> child = it->second.childFrame();
        if (child) {
            if (child->m_treeId == nodeId) {
                m_rangeByElementName.erase(it->second.name());
                it = m_elements.erase(it);
                break;
            } else {
                child->removeFEWithChild(nodeId);
            }
        }
    }
    assert(m_elements.size()-1 == m_rangeByElementName.size());
}

Frame* Frame::find(int id) {
    if (this->m_treeId == id) {
        return this;
    }
    for (const auto& child : m_elements) {
        std::shared_ptr<Frame> found = child.second.childFrame();
        if (child.second.isFrame() && found) {
            return found.get();
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

