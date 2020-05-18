#pragma once

#include <QObject>
#include <memory>
#include <map>
#include "AbstractModule.h"
#include "stwindow.h"
#include "constituency.h"
#include "dependency.h"
#include "framenet.h"
#include "framenetmodel.h"

enum SentenceState {NODATA, PARTIAL, DONE};

struct SentenceData {
    SentenceData(const Sentence& sent)
        : constituency(sent),
        dependency(sent),
        framenet(sent) {}

    ConstituencyTree constituency;
    DependencyTree dependency;
    FrameTree framenet;

    SentenceState constituencyState = NODATA;
    SentenceState dependencyState = NODATA;
    SentenceState framenetState = NODATA;
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
    const FrameNetModel& framesModel();

public slots:
    void onSentenceChanged(const Position& pos);

private:
    Feature m_feature;
    STWindow m_widget;

    Sentence m_currentSentence;
    SentenceData *m_currentSentenceData = nullptr;
    FrameNetModel m_framesModel;
    std::map<Position, std::unique_ptr<SentenceData>> m_storage;
};

