#include "framenetmodel.h"
#include "framenet.h"

QStringList FrameNetModel::frameNamesForLexicalUnit(const QString& word) const {
    if (word == "considering") {
        return { "Cogitation" };
    }
    // TODO
    return { };
}

QStringList FrameNetModel::frameElementsFor(const QString& frameName) const {
    if ("Cogitation") {
        return {"Cognizer", "Topic", "Degree", "Time"};
    }
    return {"Agent", "Figure", "Entity", "Ground", "Event", "Time"};
}
