#include "SentenceTree.h"

SentenceTree::SentenceTree() : 
    AbstractModule("SentenceTree"),
    m_constituency("Дерево составляющих", QIcon(), new QDockWidget(), this),
    m_dependency("Дерево зависимостей", QIcon(), new QDockWidget(), this),
    m_framenet("Дерево FrameNet", QIcon(), new QDockWidget(), this)
{
    m_constituency.window->setWidget(&m_constituencyWidget);
    m_dependency.window->setWidget(&m_dependencyWidget);
    m_framenet.window->setWidget(&m_framenetWidget);
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


