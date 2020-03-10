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
    void load(QDir* dir) override;
    void save(QDir& dir) override;
    void render(const Position& from, const Position& to, DomChapter& dom, const QVector<Feature*>& activeFeatures) override;

    QString data;

private slots:
    void handler();

private:
    Feature m_feat;
    EngineTestWidget m_widget;
};
