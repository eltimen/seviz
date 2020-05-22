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
