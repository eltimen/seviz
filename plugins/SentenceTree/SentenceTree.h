#pragma once

#include <QObject>
#include <memory>
#include <map>
#include "ISevizPlugin.h"
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

#if defined(SENTENCETREE_LIBRARY)
#  define SENTENCETREESHARED_EXPORT Q_DECL_EXPORT
#else
#  define SENTENCETREESHARED_EXPORT Q_DECL_IMPORT
#endif

class SENTENCETREESHARED_EXPORT SentenceTree : public ISevizPlugin
{
    Q_OBJECT
    Q_INTERFACES(ISevizPlugin)
    Q_PLUGIN_METADATA(IID "seviz.SentenceTree")

public:
    SentenceTree();
    ~SentenceTree() override;

    const QString& id() const override;
    int version() const override;
    void init(IEngine* engine) override;
    virtual QList<Feature> features() override;

    virtual void render(const Position& from, const Position& to, DomChapter& dom, const QVector<Feature*>& activeFeatures) override;

    virtual void load(QDir* moduleDir) override;
    virtual void save(QDir& moduleDir) override;

    // TODO public sentenceData(pos...)
    bool currentSentenceSelected() const;
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

