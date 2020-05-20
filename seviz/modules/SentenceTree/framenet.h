#ifndef FRAMENET_H
#define FRAMENET_H

#include <map>
#include <algorithm>
#include <QString>
#include "BookModels.h"
#include "framenetmodel.h"
#include "wordrange.h"

struct FrameInsertionData {
    bool hasSubframe = false;
};

class FrameElement;

class Frame {

public:
    Frame(const QString& name, const Word& lu, const WordRange& range, const std::vector<Word>& words, const FrameNetModel& frameNetDb);

    QString name() const;
    const Word& lu() const;
    const std::vector<Word>& words() const;
    WordRange range() const;
    QStringList elementsList() const;
    QStringList getFreeElementsList() const;

    std::map<WordRange, FrameElement>& elements();
    void setElement(const FrameElement& val);
    void setTreeId(int id);

    void toTreantJson(QString& ret, int depth, int maxDepth, const QString& parentFe = "") const;

    Frame* find(int id);
    Frame* findLeastParentForRange(const WordRange& range) const;
    int maxDepth() const;

private:
    WordRange m_range;// borders in sentence
    std::vector<Word> m_words;
    QString m_name; // name 
    const QStringList m_allowedElements; // allowed FEs
    Word m_lu; // LU - word id
    std::map<WordRange, FrameElement> m_elements; // FEs
    // TODO FE in order of sentence words (ranges?) 
    int m_treeId = -1;
};

class FrameTree {
public:
    FrameTree(const Sentence& sent);
    ~FrameTree();

    int insertFrame(Frame* frame, const QString& parentFe = "");
    Frame* findByTreeId(int id);
    bool editFrame(int nodeId);
    bool remove(int nodeId);

    bool canInsertFrameWithRange(const WordRange& range, FrameInsertionData* data = nullptr); // may be pos (to check sentence borders)

    QString toTreantJson() const;
private:
    Sentence m_sentence;
    int m_lastId = 0;

    Frame* m_rootFrame = nullptr;
    // adjacency list
    // frame name -> [ {FE, }, ...] ?
    // word_id_range -> [Frame] ?
};

#endif // FRAMENET_H
