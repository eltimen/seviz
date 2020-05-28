#ifndef FRAME_H
#define FRAME_H

#include <map>
#include <set>
#include <QString>
#include "BookModels.h"
#include "frameelement.h"
#include "framenetmodel.h"
#include "wordrange.h"

class Frame {

public:
    // range - диапазон слов в предложении, закрепленных за данным фреймом
    // words - слова, которые изначально закреплены за данным фреймом для создания вложенных в него FE
    Frame(const QString& name, const Word& lu, const WordRange& range, const std::vector<Word>& words, const FrameNetModel& frameNetDb);
    ~Frame();

    int treeId() const;
    void setTreeId(int id);

    QString name() const;
    const Word& lu() const;
    std::vector<Word> words() const;
    WordRange range() const;
    std::vector<std::shared_ptr<Frame>> subFrames() const;

    const std::map<WordRange, FrameElement>& elements() const;
    QStringList elementsList() const;
    QStringList getFreeElementsList() const;
    FrameElement& operator[] (const QString& feName);
    void setElement(const FrameElement& val);
    FrameElement takeElement(const QString& name);
    void clearElements();

    void toTreantJson(QString& ret, int depth, int maxDepth, const QString& parentFe = "") const;

    void removeFeWhichContainSubframe(int nodeId);
    Frame* find(int id);
    Frame* findLeastParentForRange(const WordRange& range) const;
    int maxDepth() const;

private:
    WordRange m_range;// borders in sentence
    std::set<Word> m_words;
    std::set<Word> m_currentWords;
    QString m_name; // name 
    const QStringList m_allowedElements; // allowed FEs
    Word m_lu; // LU - word id
    std::map<WordRange, FrameElement> m_elements; // FEs
    std::map<QString, WordRange> m_rangeByElementName; // FEs
    // TODO FE in order of sentence words (ranges?) 
    int m_treeId = -1;
};

#endif // FRAME_H
