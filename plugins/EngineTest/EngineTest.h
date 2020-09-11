#pragma once

#include <QObject>
#include "ISevizPlugin.h"
#include "enginetestwidget.h"

#if defined(ENGINETEST_LIBRARY)
#  define ENGINETESTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define ENGINETESTSHARED_EXPORT Q_DECL_IMPORT
#endif

class ENGINETESTSHARED_EXPORT EngineTest : public ISevizPlugin
{
    Q_OBJECT
    Q_INTERFACES(ISevizPlugin)
    Q_PLUGIN_METADATA(IID "seviz.EngineTest")

public:
    EngineTest();
    ~EngineTest() override;

    const QString& id() const override;
    int version() const override;
    void init(IEngine* engine) override;
    virtual QList<Feature> features() override;
    void load(QDir* dir) override;
    void save(QDir& dir) override;
    void render(const Position& from, const Position& to, DomChapter& dom, const QVector<Feature*>& activeFeatures) override;

    QString data;
    bool renderStateChanged = false;

private slots:
    void handler();

private:
    Feature m_feat;
    EngineTestWidget m_widget;
};
