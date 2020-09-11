#include "framenetmodel.h"

#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include "framenet.h"

FrameNetModel::FrameNetModel() {
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("plugins/SentenceTree_resources/framenet/framenet.db");

    if (!m_db.open()) {
        qDebug() << m_db.lastError().text();
    } else {
        qDebug() << "db ok!";
    }
}

QStringList FrameNetModel::frameNamesForLexicalUnit(const QString& word) const {
    QSqlQuery query(m_db);
    // TODO добавить POS-тег в качестве параметра
    // DISTICT ниже временно нужен для удаления одинаковых слов, у которых разная часть речи
    // TODO фильтр по части речи
    query.prepare("SELECT DISTINCT frameName FROM lexical_units "
                  "WHERE text IS :text");
    query.bindValue(":text", word);
    if (query.exec()) {
        QStringList frameNames;
        while (query.next()) {
            frameNames << query.value(0).toString();
        }
        return frameNames;
    } else {
        qDebug() << query.lastError();
        return {};
        // TODO throw
    }
}

QStringList FrameNetModel::frameElementsFor(const QString& frameName) const {
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM frame_elements "
                  "WHERE frameName IS :frameName ");
    query.bindValue(":frameName", frameName);
    if (query.exec()) {
        QStringList res;
        while (query.next()) {
            res << query.value(query.record().indexOf("name")).toString();
        }
        return res;
    } else {
        qDebug() << query.lastError();
        return {};
        // TODO throw
    }
}

QPair<QString, QString> FrameNetModel::getColorsForFE(const QString& frameName, const QString& elementName) const {
    QSqlQuery query(m_db);
    query.prepare("SELECT bgColor, fgColor FROM frame_elements "
                  "WHERE frameName IS :frameName AND name IS :feName");
    query.bindValue(":frameName", frameName);
    query.bindValue(":feName", elementName);
    if (query.exec()) {
        while (query.next()) {
            return { query.value(query.record().indexOf("bgColor")).toString(),  query.value(query.record().indexOf("fgColor")).toString() };
        }  
    } else {
        qDebug() << query.lastError();
        // TODO throw
    }
    return {};
}
