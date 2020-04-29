#include "framenetmodel.h"
#include "framenet.h"

QStringList FrameNetModel::frameNamesForLexicalUnit(const QString& word) const {
    // TODO
    return { "Compliance",  "Assistance", "Visiting", "Spatial_collocation", "Entity" };
}

QStringList FrameNetModel::frameElementsFor(const QString& frameName) const {
    return {"Agent", "Figure", "Entity", "Ground", "Event", "Time"};
}
