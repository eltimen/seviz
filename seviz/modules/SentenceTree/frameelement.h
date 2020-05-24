#ifndef FRAMEELEMENT_H
#define FRAMEELEMENT_H

#include <vector>
#include <memory>
#include "BookModels.h"
#include "wordrange.h"

class Frame;

class FrameElement {
public:
    FrameElement() = default;

    FrameElement(const QString& name, Frame* child);
    FrameElement(const QString& name, std::shared_ptr<Frame> child);

    FrameElement(const QString& name, const QList<Word>& phrase);
    ~FrameElement();

    bool isWord() const;
    bool isFrame() const;
    bool isEmpty() const;

    const QString& name() const;
    WordRange range() const;
    std::shared_ptr<Frame> childFrame() const;
    const QList<Word>& words() const;

    void toTreantJson(QString& ret, int depth, int maxDepth) const;
private:
    QString m_name;
    // value - frame or word id
    std::shared_ptr<Frame> m_subFrame = nullptr;
    QList<Word> m_words;
};

#endif // FRAMEELEMENT_H
