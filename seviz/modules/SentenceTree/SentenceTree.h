#pragma once

#include <QObject>
#include "AbstractModule.h"
#include "Feature.h"
#include "stwindow.h"
#include "dependency.h"

enum SentenceState {NODATA, PARTIAL, DONE};

struct SentenceData {
    SentenceData(const Sentence& sent) : dependency(sent) {}

    DependencyTree dependency;
    SentenceState dependencyState = NODATA;
};

class SentenceTree : public AbstractModule
{
    Q_OBJECT

public:
    SentenceTree(ModuleManager* engine);
    ~SentenceTree();

    virtual QList<Feature> features() override;

    virtual void render(const Position& from, const Position& to, DomChapter& dom, const QVector<Feature*>& activeFeatures) override;

private slots:
    void onSentenceChanged(const Position& pos);

private:
    Feature m_feature;
    STWindow m_widget;

    Sentence m_currentSentence;
    QMap<Position, SentenceData> m_storage;
};

