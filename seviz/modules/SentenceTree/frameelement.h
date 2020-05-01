#ifndef FRAMEELEMENT_H
#define FRAMEELEMENT_H

#include <vector>
#include "BookModels.h"
#include "wordrange.h"

class Frame;

class FrameElement {
public:
    FrameElement() = default;

    FrameElement(const QString& name, Frame* child)
        : m_subFrame(child),
        m_name(name)
    { }

    FrameElement(const QString& name, const QList<Word>& phrase)
        : m_words(phrase),
        m_name(name)
    {
        assert(phrase.size() > 0);
        // TODO assert слова в фразе должны быть соседями
    }

    bool isWord() const;
    bool isFrame() const;

    QString name() const;
    WordRange range() const;
    Frame* childFrame() const;
    QList<Word> words() const;

    void toTreantJson(QString& ret, int depth, int maxDepth) const;
private:
    QString m_name;
    // value - frame or word id
    Frame* m_subFrame = nullptr;
    QList<Word> m_words;
};

#endif // FRAMEELEMENT_H
