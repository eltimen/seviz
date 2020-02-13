#pragma once

#include <QObject>
#include "AbstractModule.h"
#include "Feature.h"
#include "enginetestwidget.h"

class EngineTest : public AbstractModule
{
    Q_OBJECT

public:
    EngineTest();
    ~EngineTest();

    virtual QList<Feature> features() override;

private:
    Feature m_feat;
    EngineTestWidget m_widget;
};
