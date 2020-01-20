#include "AbstractModule.h"

AbstractModule::AbstractModule(const QString& id) : 
    QObject(nullptr),
    m_id(id)
{
}

AbstractModule::~AbstractModule()
{
}

const QString& AbstractModule::id() {
    return m_id;
}
