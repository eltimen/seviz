#ifndef FRAMEELEMENT_H
#define FRAMEELEMENT_H

#include <vector>
#include "BookModels.h"
#include "wordrange.h"

class Frame;

class FrameElement {
public:
    FrameElement() = default;

    FrameElement(Frame* child)
        : m_subFrame(child) 
    { }

    FrameElement(const QList<Word>& phrase)
        : m_words(phrase) 
    {
        assert(phrase.size() > 0);
        // TODO assert слова в фразе должны быть соседями
    }

    bool isWord() const;
    bool isFrame() const;

    WordRange range() const;
    Frame* childFrame() const;
    QList<Word> words() const;

    QString toTreantJson() const;
private:
    // value - frame or word id
    Frame* m_subFrame = nullptr;
    QList<Word> m_words;
};

#endif // FRAMEELEMENT_H
