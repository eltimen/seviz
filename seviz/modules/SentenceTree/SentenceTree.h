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
    explicit SentenceData(const Sentence& sent)
        : sentence(sent),
          constituency(sent),
          dependency(sentence),
          framenet(sent) 
    {}

    Sentence sentence;
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

    virtual void load(QDir* moduleDir) override;
    virtual void save(QDir& moduleDir) override;

    // TODO public sentenceData(pos...)
    SentenceData& SentenceTree::currentSentenceData(); // TODO private, friend class
    const Position& SentenceTree::currentSentencePos() const; // TODO private, friend class
    const FrameNetModel& framesModel();

public slots:
    void onSentenceChanged(const Position& pos);
    void onRunParser();

private:
    QJsonDocument execCoreNlp();

    void loadTokensDataCoreNlp(const QString& json, Sentence& sent);
    QString tokensDataToCoreNlpJson(const Sentence& sent);

    Feature m_feature;
    STWindow m_widget;
    FrameNetModel m_framesModel;

    Position m_currentSentencePos;
    SentenceData *m_currentSentenceData = nullptr;
    std::map<Position, std::unique_ptr<SentenceData>> m_storage; 
};

