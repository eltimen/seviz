#include "ModuleManager.h"
#include <tuple>
#include <iterator>
#include "modules/modules.h"

ModuleManager::ModuleManager(EpubRenderer& render) : m_render(render) {
    for (AbstractModule* m : registrar()) {
        m->m_engine = this;
        if (!m_container.contains(m->id())) {
            m_container.insert(m->id(), m);
        } else {
            destroy();
            throw DuplicateModulesException();
        }
    }
}

ModuleManager::~ModuleManager() {
    // Qt smart pointers can't be stored in Qt containers
    destroy();
}

void ModuleManager::bookOpened(Book* book) {
    m_book = book;
}

void ModuleManager::featureEnabled(const Feature& feature) {
    qDebug() << "enabled " + feature.name;
    // TODO restore handlers & hotkeys
}

void ModuleManager::featureDisabled(const Feature& feature) {
    qDebug() << "disabled " + feature.name;
    // TODO disable handlers & hotkeys
}

void ModuleManager::forEachModule(std::function<void(AbstractModule*)> functor) {
    for (AbstractModule* i : m_container) {
        functor(i);
    }
}

void ModuleManager::destroy() {
    for (AbstractModule* i : m_container) {
        delete i;
    }
    m_container.clear();
}

AbstractModule* ModuleManager::getModule(const QString& id) {
    return m_container.value(id, nullptr);
}

AbstractModule* ModuleManager::getModule(const QString& id, int minVersion) {
    AbstractModule* m = m_container.value(id, nullptr);
    return m->version() >= minVersion ? m : nullptr;
}

const Book& ModuleManager::getBook() {
    return *m_book;
}