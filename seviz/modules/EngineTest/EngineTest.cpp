#include "EngineTest.h"
#include <QMessageBox>

EngineTest::EngineTest(ModuleManager* engine) :
    AbstractModule(engine, "EngineTest"),
    m_widget(*this),
    m_feat("Тесты движка", QIcon(), new QDockWidget(), this)
{
    m_widget.m_engine = m_engine;
    m_feat.window()->setWidget(&m_widget);
    m_feat.setDockLocation(Qt::RightDockWidgetArea);
    m_engine->registerHotkey(QKeySequence("Shift+A"), m_feat, VOIDSLOT(EngineTest::handler));
}

EngineTest::~EngineTest() {
}

QList<Feature> EngineTest::features() {
    return { 
        m_feat
    };
}

void EngineTest::load(QDir* dir) {
    if (dir) {
        QFile file(dir->filePath("data.txt"));
        if (file.open(QIODevice::ReadOnly)) {
            data = QString(file.readLine());
            m_widget.setText(data);
        }
        file.close();
    }   
}

void EngineTest::save(QDir& dir) {
    QFile file(dir.filePath("data.txt"));
    if (file.open(QIODevice::WriteOnly)) {
        file.write(data.toLocal8Bit());
    }
    file.close();
}

void EngineTest::handler() {
    QMessageBox::information(m_feat.window(), "Хоткей", "Сработал хоткей");
}


