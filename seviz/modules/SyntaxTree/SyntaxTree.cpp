#include "SyntaxTree.h"

SyntaxTree::SyntaxTree() : 
    AbstractModule("syntax-tree")
{
}

SyntaxTree::~SyntaxTree()
{
}

QList<Feature> SyntaxTree::getFeatures() {
    return { 
        Feature("show", QIcon(), "SyntaxTree", nullptr, this) 
    };
}


