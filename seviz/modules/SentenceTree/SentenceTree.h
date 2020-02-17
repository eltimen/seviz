#pragma once

#include <QObject>
#include "AbstractModule.h"
#include "Feature.h"
#include "constituencywindow.h"
#include "dependencywindow.h"
#include "framenetwindow.h"

class SentenceTree : public AbstractModule
{
    Q_OBJECT

public:
    SentenceTree(ModuleManager* engine);
    ~SentenceTree();

    virtual QList<Feature> features() override;

private:
    Feature m_constituency;
    Feature m_dependency;
    Feature m_framenet;

    ConstituencyWindow m_constituencyWidget;
    DependencyWindow m_dependencyWidget;
    FramenetWindow m_framenetWidget;
};
