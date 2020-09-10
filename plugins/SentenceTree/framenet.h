#ifndef FRAMENET_H
#define FRAMENET_H

#include <map>
#include <algorithm>
#include <QString>
#include <QDebug>
#include "BookModels.h"
#include "frame.h"

using FrameVector = std::vector<Frame*>;

// хранит информацию о ситуации при вставке фрейма
struct FrameInsertionData {
    /* Возможно 4 случая при вставке newFrame:
        1) дерева фреймов еще нет - тогда просто добавляем вставляемый фрейм в root
        2) Расширение корневого узла - во всем дереве только 1 фрейм (в root) и мы добавляем его определенный FE к newFrame
        3) Уточнение корневого узла - newFrame добавляется в определенный FE потомком у корневого узла
        4) Вставка в середину - находится фрейм верхний и нижние. 
    */
    Frame* highFrame = nullptr;
    FrameVector lowFrames;
    std::map<WordRange,FrameElement> lowFramesAsHighFrameElements;
    std::vector<QString> feNamesOfChildFramesInsideFutureParent;

    bool hasChildFrames() const {
        return lowFrames.size() > 0;
    }
};

class FrameElement;

class FrameTree {
public:
    explicit FrameTree(const Sentence& sent);
    ~FrameTree();

    int insertFrame(Frame* frame, const QString& parentFe = "", const std::map<QString, QString>& childFramesInitFE = {});
    Frame* findByTreeId(int id);
    void remove(int nodeId);

    bool canInsertFrameWithRange(const WordRange& range, FrameInsertionData* data = nullptr); // may be pos (to check sentence borders)

    void fromJson(const QString& json, const FrameNetModel& frameNetDb, IEngine* engine, const Position& currentSentencePos);
    QString toJson() const;

    QString toTreantJson() const;
private:
    Sentence m_sentence;
    int m_lastId = 0;

    Frame* m_rootFrame = nullptr;
};

#endif // FRAMENET_H
