#pragma once

#include <QObject>
#include "AbstractModule.h"
#include "Feature.h"
#include "enginetestwidget.h"

class EngineTest : public AbstractModule
{
    Q_OBJECT

public:
    EngineTest(ModuleManager* engine);
    ~EngineTest();

    virtual QList<Feature> features() override;
    void load(QDir& dir) override;
    void save(QDir& dir) override;

    QString data;

private:
    Feature m_feat;
    EngineTestWidget m_widget;
};
