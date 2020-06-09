#include "framenet.h"

#include "frameelement.h"

// ---------- FrameNet tree methods ------------------------------

FrameTree::FrameTree(const Sentence& sent)
    : m_sentence(sent)
{
}

FrameTree::~FrameTree() { 
    delete m_rootFrame;
}

int FrameTree::insertFrame(Frame* frame, const QString& parentFe, const std::map<QString, QString>& childFramesInitFE) {
    // TODO отбирать слова из диапазона у фрейма верхнего уровня
    frame->setTreeId(++m_lastId);
    if (!m_rootFrame) {
        m_rootFrame = frame;
    } else if(m_rootFrame->range().isInsideOf(frame->range())) {
        assert(childFramesInitFE.size() == 1);
        // это расширение корневого фрейма? 
        frame->setElement(FrameElement(childFramesInitFE.begin()->first, m_rootFrame));
        m_rootFrame = frame;
    } else if (m_rootFrame->range().isOutsideOf(frame->range())) {
        // это уточнение корневого фрейма?
        // найти самый узкий родительский фрейм (проверяя на пересечения)
        Frame* parentToInsert = m_rootFrame->findLeastParentForRange(frame->range());
        // забрать у него дочерние FE, которые входят в диапазон добавляемого фрейма
        // и передать их в добавляемый фрейм
        for (const auto& feAndParentFe : childFramesInitFE) {
            FrameElement removedFe = parentToInsert->takeElement(feAndParentFe.second);
            assert(removedFe.isFrame());
            frame->setElement(FrameElement(feAndParentFe.first, removedFe.childFrame()));
        }
        // добавить добавляемый фрейм его родителю как потомка
        parentToInsert->setElement(FrameElement(parentFe, frame));
        
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

void FrameTree::remove(int nodeId) {
    if (m_rootFrame->treeId() != nodeId) {
        m_rootFrame->removeFeWhichContainSubframe(nodeId);
    } else {
        delete m_rootFrame;
        m_rootFrame = nullptr;
    }
    
}

bool FrameTree::canInsertFrameWithRange(const WordRange& range, FrameInsertionData* data) {
    bool res = false;
    // true, если дерево фреймов пустое 
    if (!m_rootFrame) {
        res = true;
    // или диапазон включает в себя диапазон родительского фрейма
    } else if (m_rootFrame->range().isInsideOf(range)) {
        res = true;
        if (data) {
            data->lowFrames = { m_rootFrame };
        }
    // или он входит в корневой фрейм 
    } else if (m_rootFrame->range().isOutsideOf(range)) { //TODO
        res = true;
        Frame* parentToInsert = m_rootFrame->findLeastParentForRange(range);
        if (range.contains(parentToInsert->lu().id())) {
            res = false;
        } else if (data) {
            data->highFrame = parentToInsert;
            for (const auto& fe : parentToInsert->elements()) {
                if (fe.second.isFrame() && fe.first.isInsideOf(range)) {
                    data->lowFrames.push_back(fe.second.childFrame().get());
                    data->feNamesOfChildFramesInsideFutureParent.push_back(fe.second.name());
                }
            }
        }
        //    и вставляемый диапазон не пересекается с диапазоном ни одного фрейма в дереве
        //      и вставляемый диапазон свободен (не занят LU и не входит в какие-нибудь FE)
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
