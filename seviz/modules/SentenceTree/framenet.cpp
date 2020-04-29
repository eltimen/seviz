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
        frame->setElement(fe, m_rootFrame);
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

// --------- frame classes -------------------------------------------------------

Frame::Frame(const QString& name, int luWordId, const WordRange& range, const FrameNetModel& frameNetDb) 
    : m_name(name),
    m_lu(luWordId),
    m_allowedElements(frameNetDb.frameElementsFor("name")),
    m_range(range)
{
}

WordRange Frame::range() const {
    return m_range;
}

void Frame::setElement(const QString& name, const FrameElement& val) {
    assert(m_allowedElements.contains(name));
    assert(!val.range().contains(m_lu));

    m_elements[name] = val;
}

Frame* Frame::findParentToInsertFrame(const WordRange& range) {
    Frame* found = this;
    for (const std::pair<QString, FrameElement>& fe : m_elements) {
        if (fe.second.isFrame() && fe.second.childFrame()->range().isInsideOf(range)) {
            found = fe.second.childFrame();
            break;
        }
    }

    return m_range.isInsideOf(range) 
        ? found
        : nullptr;
}
