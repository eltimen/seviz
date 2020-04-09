#include "AbstractModule.h"
#include "ModuleManager.h"

AbstractModule::AbstractModule(IEngine* engine, const QString& id) :
    QObject(nullptr),
    m_id(id),
    m_engine(engine)
{
}

AbstractModule::~AbstractModule() {
}

const QString& AbstractModule::id() const {
    return m_id;
}

int AbstractModule::version() const {
    return 0;
}

#pragma warning(disable:4100)

void AbstractModule::load(QDir* moduleDir) {
    qDebug() << (moduleDir ? moduleDir->path() + " load existing" : "loading empty module dir");
}


void AbstractModule::save(QDir& moduleDir) {}

void AbstractModule::render(const Position& from, const Position& to, DomChapter& dom, const QVector<Feature*>& activeFeatures) {}


