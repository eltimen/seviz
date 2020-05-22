#ifndef FRAMENETMODEL_H
#define FRAMENETMODEL_H

#include <memory>
#include <QSqlDatabase>
#include <QString>
#include <QStringList>

class FrameNetModel {
public:
    FrameNetModel();

    QStringList frameNamesForLexicalUnit(const QString& word) const;
    QStringList frameElementsFor(const QString& frameName) const;
    
private: 
    QSqlDatabase m_db;
};


#endif // FRAMENETMODEL_H
