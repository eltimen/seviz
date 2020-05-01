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
    Frame(const QString& name, const Word& lu, const WordRange& range, const FrameNetModel& frameNetDb);

    WordRange range() const;
    void setElement(const FrameElement& val);

    Frame* findParentToInsertFrame(const WordRange& range);

    void toTreantJson(QString& ret, int depth, int maxDepth, const QString& parentFe = "") const;
    int maxDepth() const;
private:
    WordRange m_range;// borders in sentence

    QString m_name; // name 
    const QStringList m_allowedElements; // allowed FEs

    Word m_lu; // LU - word id
    std::map<WordRange, FrameElement> m_elements; // FEs
    // TODO FE in order of sentence words (ranges?)
};

class FrameTree {
public:
    FrameTree(const Sentence& sent);
    ~FrameTree();

    int insertFrame(Frame* frame, const QString& parentFe = "");
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
