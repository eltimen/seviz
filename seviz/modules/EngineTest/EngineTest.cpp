#include "EngineTest.h"

EngineTest::EngineTest(ModuleManager* engine) :
    AbstractModule(engine, "EngineTest"),
    m_feat("Тесты движка", QIcon(), new QDockWidget(), this)
{
    m_widget.m_engine = m_engine;
    m_feat.window->setWidget(&m_widget);
}

EngineTest::~EngineTest() {
}

QList<Feature> EngineTest::features() {
    return { 
        m_feat
    };
}


