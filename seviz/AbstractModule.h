#pragma once
#include <QObject>
#include <QSet>
#include <QString>
#include <QDir>
#include "Feature.h"
#include "BookModels.h"
#include "DomChapter.h"

class ModuleManager;

class AbstractModule : public QObject
{
    Q_OBJECT

public:
    AbstractModule(ModuleManager* engine, const QString& id);
    virtual ~AbstractModule();

    virtual const QString& id() const;
    virtual int version() const;
    virtual QList<Feature> features() = 0;

    virtual void load(QDir* moduleDir);
    virtual void save(QDir& moduleDir);

public slots:
    virtual void render(const Position& from, const Position& to, DomChapter& dom, const QVector<Feature*>& activeFeatures);

protected:
    ModuleManager* m_engine = nullptr;

private:
    QString m_id;
};
