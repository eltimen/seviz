#pragma once
#include <QObject>
#include <QSet>
#include <QString>
#include "Feature.h"

class AbstractModule : public QObject
{
    Q_OBJECT

public:
    AbstractModule(const QString& id);
    virtual ~AbstractModule();

    const QString& id();
    virtual QList<Feature> features() = 0;

public slots:
    //virtual void featureTriggered(const QString& id) = 0;

protected:
    QString m_id;
};
