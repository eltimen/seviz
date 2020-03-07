#pragma once

#include <QObject>
#include "AbstractModule.h"
#include "Feature.h"
#include "stwindow.h"
#include "dependency.h"

struct SentenceData {
    SentenceData(const Sentence& sent) : dependency(sent) {}

    DependencyTree dependency;
};

class SentenceTree : public AbstractModule
{
    Q_OBJECT

public:
    SentenceTree(ModuleManager* engine);
    ~SentenceTree();

    virtual QList<Feature> features() override;

private slots:
    void onSentenceChanged(const Position& pos);

private:
    Feature m_feature;
    STWindow m_widget;

    Sentence m_currentSentence;
    QMap<Position, SentenceData> m_storage;
};

