#pragma once

#include <QObject>
#include "AbstractModule.h"
#include "enginetestwidget.h"

class EngineTest : public AbstractModule
{
    Q_OBJECT

public:
    EngineTest(IEngine* engine);
    ~EngineTest();

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
    Feature m_feat2;
    EngineTestWidget m_widget;
};
