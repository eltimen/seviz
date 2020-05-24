#include "framenetmodel.h"

#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include "framenet.h"

FrameNetModel::FrameNetModel() {
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("SentenceTree_resources/framenet/framenet.db");

    if (!m_db.open()) {
        qDebug() << m_db.lastError().text();
    } else {
        qDebug() << "db ok!";
    }
}

QStringList FrameNetModel::frameNamesForLexicalUnit(const QString& word) const {
    QSqlQuery query(m_db);
    // TODO добавить POS-тег в качестве параметра
    // DISTICT ниже нужен для удаления одинаковых слов, у которых разная часть речи
    query.prepare("SELECT DISTINCT frameName FROM lexical_units "
                  "WHERE text IS :text");
    query.bindValue(":text", word);
    if (query.exec()) {
        QStringList res;
        while (query.next()) {
            res << query.value(0).toString();
        }
        return res;
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
