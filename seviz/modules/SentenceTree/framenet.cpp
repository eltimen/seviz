#include "framenet.h"

#include <QSet>
#include "frameelement.h"

// ---------- FrameNet tree methods ------------------------------

FrameTree::FrameTree(const Sentence& sent)
    : m_sentence(sent)
{
}

FrameTree::~FrameTree() {
}

int FrameTree::insertFrame(Frame* frame, const QString& fe) {
    // TODO отбирать слова из дипапзона у фрейма верхнего уровня
    frame->setTreeId(++m_lastId);
    if (!m_rootFrame) {
        assert(fe.isEmpty());
        m_rootFrame = frame;
    } else if(m_rootFrame->range().isInsideOf(frame->range())) {
        // это расширение корневого фрейма? 
        frame->setElement(FrameElement(fe, m_rootFrame));
        m_rootFrame = frame;
    } else if (m_rootFrame->range().isOutsideOf(frame->range())) {
        // это уточнение корневого фрейма?
        // найти самый узкий родительский фрейм (проверяя на пересечения)
        //  m_rootFrame->setElement(parentFE, FrameElement(frame));
        //
    } else {
        // попытка добавить фрейм на одном уровне с корневым?
        // в этой версии такое не реализовано
        throw -1;
    }
    return m_lastId;
}

Frame* FrameTree::findByTreeId(int id) {
    return m_rootFrame->find(id);
}

bool FrameTree::canInsertFrameWithRange(const WordRange& range, FrameInsertionData* data) {
    bool res = false;
    // true, если дерево фреймов пустое 
    // или диапазон включает в себя диапазон родительского фрейма
    // или в дереве есть фрейм, диапазон которого включает в себя данный фрейм 
    //      и вставляемый диапазон не пересекается с диапазоном ни одного фрейма в дереве
    //      и вставляемый диапазон свободен (не занят LU и не входит в какие-нибудь FE)
    if (!m_rootFrame) {
        res = true;
    } else if (m_rootFrame->range().isInsideOf(range)) {
        res = true;
        if (data) {
            data->hasSubframe = true;
        }
    } else if (false) { //TODO
    }

    return res;
}

QString FrameTree::toTreantJson() const {
    QString ret = QStringLiteral(R"(
    {
    "chart": {
        "container": "#OrganiseChart-simple",
        "levelSeparation": 14,
        "siblingSeparation": 5,
        "subTeeSeparation": 10,
        "connectors": {
            "type": "step",
            "style": { "arrow-end": "open-wide-long" }
        }
    },
    "nodeStructure": 
    )");

    if (!m_rootFrame) {
        ret += "{}";
    } else {
        m_rootFrame->toTreantJson(ret, 0, m_rootFrame->maxDepth());
    }
    ret += "}";
    return ret;
}

// --------- frame classes -------------------------------------------------------

Frame::Frame(const QString& name, const Word& lu, const WordRange& range, const std::vector<Word>& words, const FrameNetModel& frameNetDb)
    : m_name(name),
    m_lu(lu),
    m_words(words),
    m_allowedElements(frameNetDb.frameElementsFor("name")),
    m_range(range)
{
    // для упрощения вывода дерева LU заносится как FE "LU"
    assert(WordRange(lu.id(), lu.id()) == FrameElement("LU", { lu }).range());
    m_elements.emplace(WordRange(lu.id(), lu.id()), FrameElement("LU", {lu}));
}

QString Frame::name() const {
    return m_name;
}

const Word& Frame::lu() const {
    return m_lu;
}

const std::vector<Word>& Frame::words() const {
    return m_words;
}

WordRange Frame::range() const {
    return m_range;
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
    return m_elementsByNames[feName];
}

void Frame::clearElements() {
    // std::remove_if not works with map
    for (auto iter =  m_elements.begin(); iter != m_elements.end(); ) {
        if (iter->second.name() != "LU") 
            iter = m_elements.erase(iter);
        else
            ++iter;
    }
    m_elementsByNames.clear();
}

void Frame::setElement(const FrameElement& val) {
    assert(m_allowedElements.contains(val.name()));
    assert(!val.range().contains(m_lu.id()));

    m_elements[val.range()] = val;
    m_elementsByNames[val.name()] = val;
}

void Frame::setTreeId(int id) {
    m_treeId = id;
}

//Frame* Frame::findParentToInsertFrame(const WordRange& range) {
//    Frame* found = this;
//    for (const std::pair<WordRange, FrameElement>& fe : m_elements) {
//        if (fe.second.isFrame() && fe.second.childFrame()->range().isInsideOf(range)) {
//            found = fe.second.childFrame();
//            break;
//        }
//    }
//
//    return m_range.isInsideOf(range) 
//        ? found
//        : nullptr;
//}

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

Frame* Frame::find(int id) {
    if (this->m_treeId == id) {
        return this;
    }
    for (const auto& child : m_elements) {
        Frame* found = child.second.childFrame();
        if (child.second.isFrame() && found) {
            return found;
        }
    }
    return nullptr;
}

Frame* Frame::findLeastParentForRange(const WordRange& range) const {
    const Frame* found = this;
    for (const std::pair<WordRange, FrameElement>& fe : m_elements) {
        if (fe.second.isFrame() && fe.first.isInsideOf(range)) {
            found = fe.second.childFrame()->findLeastParentForRange(range);
            break;
        }
    }
    //assert(!found->m_isTerminal);
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
