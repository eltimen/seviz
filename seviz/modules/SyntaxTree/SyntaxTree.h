#pragma once

#include <QObject>
#include "AbstractModule.h"
#include "Feature.h"
#include "syntaxtreewindow.h"

class SyntaxTree : public AbstractModule
{
    Q_OBJECT

public:
    SyntaxTree();
    ~SyntaxTree();

    virtual QList<Feature> features() override;

private:
    QDockWidget* m_dock;
    SyntaxTreeWindow m_widget;
};
