#pragma once

#include <QObject>
#include "AbstractModule.h"
#include "Feature.h"
#include "stwindow.h"

class SentenceTree : public AbstractModule
{
    Q_OBJECT

public:
    SentenceTree(ModuleManager* engine);
    ~SentenceTree();

    virtual QList<Feature> features() override;

private:
    Feature m_feature;
    STWindow m_widget;
};
