#include "SentenceTree.h"
#include "ModuleManager.h"

SentenceTree::SentenceTree(ModuleManager* engine) : 
    AbstractModule(engine, "SentenceTree"),
    m_feature("Деревья предложений", QIcon(":/st/icon.png"), new QDockWidget(), this)
{
    m_feature.window()->setWidget(&m_widget);
    //m_engine->registerHotkey(QKeySequence("Shift+A"), m_constituency, []() {});
}

SentenceTree::~SentenceTree() {
}

QList<Feature> SentenceTree::features() {
    return { 
        m_feature
    };
}


