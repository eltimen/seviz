#pragma once

#include <QObject>
#include "AbstractModule.h"
#include "stwindow.h"
#include "constituency.h"
#include "dependency.h"

enum SentenceState {NODATA, PARTIAL, DONE};

struct SentenceData {
    SentenceData(const Sentence& sent)
        : constituency(sent),
        dependency(sent) {}

    ConstituencyTree constituency;
    DependencyTree dependency;

    SentenceState constituencyState = NODATA;
    SentenceState dependencyState = NODATA;
};

class SentenceTree : public AbstractModule
{
    Q_OBJECT

public:
    SentenceTree(IEngine* engine);
    ~SentenceTree();

    virtual QList<Feature> features() override;
    virtual void render(const Position& from, const Position& to, DomChapter& dom, const QVector<Feature*>& activeFeatures) override;

    SentenceData& SentenceTree::currentSentenceData();

private slots:
    void onSentenceChanged(const Position& pos);

private:
    Feature m_feature;
    STWindow m_widget;

    Sentence m_currentSentence;
    SentenceData *m_currentSentenceData = nullptr;
    //QMap<Position, SentenceData> m_storage;
};

