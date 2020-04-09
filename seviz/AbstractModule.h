#pragma once
#include <QObject>
#include <QSet>
#include <QString>
#include <QDir>
#include "IEngine.h"

class IEngine;

class AbstractModule : public QObject
{
    Q_OBJECT

public:
    AbstractModule(IEngine* engine, const QString& id);
    virtual ~AbstractModule();

    virtual const QString& id() const;
    virtual int version() const;
    virtual QList<Feature> features() = 0;

    virtual void load(QDir* moduleDir);
    virtual void save(QDir& moduleDir);

public slots:
    virtual void render(const Position& from, const Position& to, DomChapter& dom, const QVector<Feature*>& activeFeatures);

protected:
    IEngine* m_engine = nullptr;

private:
    QString m_id;
};
