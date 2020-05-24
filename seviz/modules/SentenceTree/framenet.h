#ifndef FRAMENET_H
#define FRAMENET_H

#include <map>
#include <algorithm>
#include <QString>
#include "BookModels.h"
#include "frame.h"

struct FrameInsertionData {
    bool hasSubframe = false;
};

class FrameElement;

class FrameTree {
public:
    FrameTree(const Sentence& sent);
    ~FrameTree();

    int insertFrame(Frame* frame, const QString& parentFe = "");
    Frame* findByTreeId(int id);
    void remove(int nodeId);

    bool canInsertFrameWithRange(const WordRange& range, FrameInsertionData* data = nullptr); // may be pos (to check sentence borders)

    QString toTreantJson() const;
private:
    Sentence m_sentence;
    int m_lastId = 0;

    Frame* m_rootFrame = nullptr;
};

#endif // FRAMENET_H
