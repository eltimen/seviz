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

void ModuleManager::registerHotkey(const QKeySequence& hotkey, const Feature& feature, const std::function<void()>& slot) {
    // проверка, что это сочетание клавиш не активно сейчас
    bool hotkeyFree = true;
    for (decltype(m_hotkeys)::iterator it = m_hotkeys.begin(); it != m_hotkeys.end() && hotkeyFree; ++it) {
        if ((*it)->isEnabled()) {
            hotkeyFree = false;
        }
    }

    if (hotkeyFree) {
        qDebug() << feature.window->parentWidget();
        QShortcut* sh = new QShortcut(hotkey, m_window, nullptr, nullptr, Qt::ApplicationShortcut);
        sh->setEnabled(false);
        qDebug() << "inactive";
        connect(sh, &QShortcut::activated, slot);
        m_hotkeys.insert(feature, sh);
    } else {
        // TODO exception
    }
   
}

void ModuleManager::featureEnabled(const Feature& feature) {
    // activating hotkeys for feature
    for (auto& i : m_hotkeys.values(feature)) {
        i->setEnabled(true);
        qDebug() << "active";
    }

    // TODO activate handlers
}

void ModuleManager::featureDisabled(const Feature& feature) {
    for (auto& i : m_hotkeys.values(feature)) {
        i->setEnabled(false);
        qDebug() << "inactive";
    }

    // TODO disable handlers & hotkeys
}
