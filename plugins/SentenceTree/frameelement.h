#ifndef FRAMEELEMENT_H
#define FRAMEELEMENT_H

#include <vector>
#include <memory>
#include <QJsonValue>
#include "BookModels.h"
#include "wordrange.h"

class Frame;
class IEngine;
class FrameNetModel;

class FrameElement {
public:
    FrameElement() = default;

    FrameElement(const QString& name, Frame* child);
    FrameElement(const QString& name, std::shared_ptr<Frame> child);

    FrameElement(const QString& name, const std::vector<Word>& phrase);
    ~FrameElement();

    bool isWord() const;
    bool isFrame() const;
    bool isEmpty() const;

    const QString& name() const;
    WordRange range() const;
    std::shared_ptr<Frame> childFrame() const;
    const std::vector<Word>& words() const;

    static FrameElement fromJson(const QJsonValue& json, const FrameNetModel& frameNetDb, IEngine* engine, const Position& currentSentencePos);
    QString toJson() const;

    void toTreantJson(QString& ret, int depth, int maxDepth, const QPair<QString, QString>& colors) const;
private:
    QString m_name;
    // value - frame or word id
    std::shared_ptr<Frame> m_subFrame = nullptr;
    std::vector<Word> m_words;
};

#endif // FRAMEELEMENT_H
