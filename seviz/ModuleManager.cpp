#include "mainwindow.h"
#include <tuple>
#include <iterator>
#include "modules/modules.h"

ModuleManager::ModuleManager(EpubRenderer& render, MainWindow* w) : 
    m_render(render),
    m_window(w) {
    for (AbstractModule* m : registrar()) {
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

void ModuleManager::forEachModule(std::function<void(AbstractModule*)> functor) {
    for (AbstractModule* i : m_container) {
        functor(i);
    }
}

QList<Feature*> ModuleManager::getConflictFeaturesFor(const Feature& f) {
    // для каждого хоткея из f
    for (decltype(m_hotkeys)::iterator it = m_hotkeys.begin(); it != m_hotkeys.end(); ++it) {
        // ищем его среди относящихся к другой функции
        // TODO
    }

    // TODO аналогично для обработчиков

    return {};
}

void ModuleManager::destroy() {
    for (AbstractModule* i : m_container) {
        delete i;
    }
    m_container.clear();
}

AbstractModule* ModuleManager::getModule(const QString& id, int minVersion) {
    AbstractModule* m = m_container.value(id, nullptr);
    return m->version() >= minVersion ? m : nullptr;
}

const Book& ModuleManager::getBook() {
    return *m_book;
}

QList<Feature*> ModuleManager::featureEnabled(const Feature& feature) {
    QList<Feature*> conflicts = getConflictFeaturesFor(feature);

    for (auto& i : m_hotkeys.values(feature)) {
        i->setEnabled(true);
    }

    for (auto& h : m_handlers.values(feature)) {
        m_render.addHandler(h.first);
        h.second = true;
    }

    return conflicts;
}

void ModuleManager::featureDisabled(const Feature& feature) {
    for (auto& i : m_hotkeys.values(feature)) {
        i->setEnabled(false);
    }

    for (auto& h : m_handlers.values(feature)) {
        m_render.removeHandler(h.first);
        h.second = false;
    }
}

void ModuleManager::registerHandler(EventType onEvent, ElementType onElements, Button withKey, const Feature& feature, const std::function<void(const Position&)>& slot) {
    Handler h(onEvent, onElements, withKey, slot);

    // TODO проверка

    m_handlers.insert(feature, qMakePair(h, false));
}

void ModuleManager::registerHotkey(const QKeySequence& hotkey, const Feature& feature, const std::function<void()>& slot) {
    // TODO проверить, что модуль не регистрирует один и тот же хоткей дважды

    QShortcut* sh = new QShortcut(hotkey, m_window, nullptr, nullptr, Qt::ApplicationShortcut);
    sh->setEnabled(false);
    connect(sh, &QShortcut::activated, slot);
    m_hotkeys.insert(feature, sh);
}

