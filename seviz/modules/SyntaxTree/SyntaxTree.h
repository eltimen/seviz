#pragma once

#include <QObject>
#include "AbstractModule.h"
#include <Feature.h>

class SyntaxTree : public AbstractModule
{
    Q_OBJECT

public:
    SyntaxTree();
    ~SyntaxTree();

    virtual QList<Feature> getFeatures() override;
};
