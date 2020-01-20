#pragma once
#include <QObject>
#include <QHash>
#include <QList>
#include <memory>
#include <functional>
#include "AbstractModule.h"
#include "exceptions.h"

class ModuleManager : public QObject
{
    Q_OBJECT

public:
    ModuleManager();
    ~ModuleManager();

    AbstractModule& getModuleByName(const QString& name);

    void forEachModule(std::function<void(AbstractModule*)> functor);
    
private:
    QHash<QString,AbstractModule*> m_container;
    void destroy();
};
