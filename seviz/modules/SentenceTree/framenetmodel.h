#ifndef FRAMENETMODEL_H
#define FRAMENETMODEL_H

#include <memory>
#include <QString>
#include <QStringList>

class FrameNetModel {
public:
    QStringList frameNamesForLexicalUnit(const QString& word) const;
    QStringList frameElementsFor(const QString& frameName) const;
    // TODO ??? get FEs for word

private: 
    // SQL database
    // pre-parsed JSON
    // raw FrameNet xmls?
};


#endif // FRAMENETMODEL_H
