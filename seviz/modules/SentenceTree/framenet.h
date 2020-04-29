#ifndef FRAMENET_H
#define FRAMENET_H

#include <map>
#include <algorithm>
#include <QString>
#include "BookModels.h"
#include "framenetmodel.h"
#include "wordrange.h"

class FrameElement;

class Frame {

public:
    Frame(const QString& name, int luWordId, const WordRange& range, const FrameNetModel& frameNetDb);

    WordRange range() const;
    void setElement(const QString& name, const FrameElement& val);

    Frame* findParentToInsertFrame(const WordRange& range);

private:
    WordRange m_range;// borders in sentence

    QString m_name; // name 
    const QStringList m_allowedElements; // allowed FEs

    int m_lu; // LU - word id
    std::map<QString, FrameElement> m_elements; // FEs
};

class FrameTree {
public:
    FrameTree(const Sentence& sent);
    ~FrameTree();

    int insertFrame(Frame* frame, const QString& fe = "");
    bool editFrame(int nodeId);
    bool remove(int nodeId);

    bool canInsertFrameWithRange(const WordRange& range); // may be pos (to check sentence borders)

    QString toTreantJson() const;
private:
    Sentence m_sentence;

    Frame* m_rootFrame = nullptr;

    // adjacency list
    // frame name -> [ {FE, }, ...] ?
    // word_id_range -> [Frame] ?
};

#endif // FRAMENET_H
