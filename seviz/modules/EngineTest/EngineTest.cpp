#include "EngineTest.h"

EngineTest::EngineTest() :
    AbstractModule("EngineTest"),
    m_feat("Тесты движка", QIcon(), new QDockWidget(), this)
{
    m_feat.window->setWidget(&m_widget);

}

EngineTest::~EngineTest() {
}

QList<Feature> EngineTest::features() {
    return { 
        m_feat
    };
}


