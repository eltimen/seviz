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
    // (bgColor, fgColor) в формате вида "FF0000"
    QPair<QString, QString> getColorsForFE(const QString& frameName, const QString& elementName) const;
    
private: 
    QSqlDatabase m_db;
};


#endif // FRAMENETMODEL_H
