#include "SyntaxTree.h"

SyntaxTree::SyntaxTree() : 
    AbstractModule("SyntaxTree")
{
    m_dock = new QDockWidget(m_id);
    m_dock->setWidget(&m_widget);
}

SyntaxTree::~SyntaxTree() {
}

QList<Feature> SyntaxTree::features() {
    return { 
        Feature("show", QIcon(), m_id, m_dock, this) 
    };
}


