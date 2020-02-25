#include "SentenceTree.h"
#include "ModuleManager.h"

SentenceTree::SentenceTree(ModuleManager* engine) : 
    AbstractModule(engine, "SentenceTree"),
    m_constituency("Дерево составляющих", QIcon(), new QDockWidget(), this),
    m_dependency("Дерево зависимостей", QIcon(), new QDockWidget(), this),
    m_framenet("Дерево FrameNet", QIcon(), new QDockWidget(), this)
{
    m_constituency.window()->setWidget(&m_constituencyWidget);
    m_dependency.window()->setWidget(&m_dependencyWidget);
    m_framenet.window()->setWidget(&m_framenetWidget);

    //m_engine->registerHotkey(QKeySequence("Shift+A"), m_constituency, []() {});
}

SentenceTree::~SentenceTree() {
}

QList<Feature> SentenceTree::features() {
    return { 
        m_constituency,
        m_dependency,
        m_framenet
    };
}


