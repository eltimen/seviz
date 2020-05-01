#include "framenet.h"
#include "frameelement.h"

// ---------- FrameNet tree methods ------------------------------

FrameTree::FrameTree(const Sentence& sent)
    : m_sentence(sent)
{
}

FrameTree::~FrameTree() {
}

int FrameTree::insertFrame(Frame* frame, const QString& fe) {
    if (!m_rootFrame) {
        assert(fe.isEmpty());
        m_rootFrame = frame;
    } else if(m_rootFrame->range().isInsideOf(frame->range())) {
        // это расширение корневого фрейма? 
        frame->setElement(FrameElement(fe, m_rootFrame));
        m_rootFrame = frame;
    } else if (false) {
        // это вставка внутрь корневого фрейма?
        //  m_rootFrame->setElement(parentFE, FrameElement(frame));
    } else {
        throw -1;
    }
    return 0;
}

bool FrameTree::canInsertFrameWithRange(const WordRange& range) {
    // true, если дерево фреймов пустое 
    // или диапазон включает в себя диапазон родительского фрейма
    // или в дереве есть фрейм, диапазон которого включает в себя данный фрейм 
    //      и вставляемый диапазон не пересекается с диапазоном ни одного фрейма в дереве
    //      и вставляемый диапазон свободен (не занят LU и не входит в какие-нибудь FE)
    return !m_rootFrame || m_rootFrame->range().isOutsideOf(range) ||
        false; // TODO
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
        ret += "{";
    } else {
        m_rootFrame->toTreantJson(ret, 0, m_rootFrame->maxDepth());
    }
    ret += "}";
    return ret;
}

// --------- frame classes -------------------------------------------------------

Frame::Frame(const QString& name, const Word& lu, const WordRange& range, const FrameNetModel& frameNetDb) 
    : m_name(name),
    m_lu(lu),
    m_allowedElements(frameNetDb.frameElementsFor("name")),
    m_range(range)
{
    // для упрощения вывода дерева LU заносится как FE "LU"
    assert(WordRange(lu.id(), lu.id()) == FrameElement("LU", { lu }).range());
    m_elements.emplace(WordRange(lu.id(), lu.id()), FrameElement("LU", {lu}));
}

WordRange Frame::range() const {
    return m_range;
}

void Frame::setElement(const FrameElement& val) {
    assert(m_allowedElements.contains(val.name()));
    assert(!val.range().contains(m_lu.id()));

    m_elements[val.range()] = val;
}

Frame* Frame::findParentToInsertFrame(const WordRange& range) {
    Frame* found = this;
    for (const std::pair<WordRange, FrameElement>& fe : m_elements) {
        if (fe.second.isFrame() && fe.second.childFrame()->range().isInsideOf(range)) {
            found = fe.second.childFrame();
            break;
        }
    }

    return m_range.isInsideOf(range) 
        ? found
        : nullptr;
}

void Frame::toTreantJson(QString& ret, int depth, int maxDepth, const QString& parentFe) const {
    ret += QStringLiteral(R"(
        {
        "text": { title: "%1", name: "%2" }, 
        "HTMLclass": "frame",
        "children": [
    )").arg(parentFe)
        .arg(m_name);

    for (const std::pair<WordRange, FrameElement>& fe : m_elements) {
        fe.second.toTreantJson(ret, depth + 1, maxDepth);
        ret += ",";
    }

    ret += "]} \n";
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
