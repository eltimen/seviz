#pragma once
#include <QObject>
#include <QSet>
#include <QString>
#include <QDir>
#include "IEngine.h"

class IEngine;

Q_DECLARE_INTERFACE(ISevizPlugin, "seviz.ISevizPlugin")
class SEVIZSHARED_EXPORT ISevizPlugin : public QObject
{
    Q_OBJECT

public:
    ISevizPlugin() : QObject(nullptr) {}
    virtual ~ISevizPlugin() {}

    virtual const QString& id() const = 0;
    virtual int version() const = 0;
    virtual void init(IEngine* engine) = 0;
    virtual QList<Feature> features() = 0;

    virtual void load(QDir* moduleDir) = 0;
    virtual void save(QDir& moduleDir) = 0;

public slots:
    virtual void render(const Position& from, const Position& to, DomChapter& dom, const QVector<Feature*>& activeFeatures) = 0;

protected:
    IEngine* m_engine = nullptr;
};
