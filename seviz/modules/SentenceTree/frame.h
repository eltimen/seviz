#ifndef FRAME_H
#define FRAME_H

#include <map>
#include <QString>
#include "BookModels.h"
#include "frameelement.h"
#include "framenetmodel.h"
#include "wordrange.h"

class Frame {

public:
    Frame(const QString& name, const Word& lu, const WordRange& range, const std::vector<Word>& words, const FrameNetModel& frameNetDb);
    void setTreeId(int id);

    QString name() const;
    const Word& lu() const;
    const std::vector<Word>& words() const;
    WordRange range() const;

    QStringList elementsList() const;
    QStringList getFreeElementsList() const;
    FrameElement& operator[] (const QString& feName);
    void setElement(const FrameElement& val);
    void clearElements();

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
    std::map<QString, FrameElement> m_elementsByNames; // FEs
    // TODO FE in order of sentence words (ranges?) 
    int m_treeId = -1;
};

#endif // FRAME_H
