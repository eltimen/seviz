#include "mainwindow.h"
#include <tuple>
#include <iterator>
#include "DomChapter.h"
#include "modules/modules.h"

ModuleManager::ModuleManager(EpubRenderer& render, MainWindow* w)
    : m_render(render),
      m_window(w),
      m_loader(this) {
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

void ModuleManager::bookOpened(Book* book, QTemporaryDir& epubDir, QList<Chapter>& chapters) {
    m_book = book;

    try {
        QDir dir(epubDir.path());
        if (dir.cd("seviz") && dir.cd(m_loader.id())) {
            // подгрузка преобразованных глав
            m_loader.load(&dir);
        } else { 
            // иначе если файл с книгой не содержит корпусных данных, вызываем загрузчик 
            dir.mkdir("seviz");
            dir.cd("seviz");
            dir.mkdir(m_loader.id());
            dir.cd(m_loader.id());

            m_loader.importBook(chapters, dir, m_window);
        }
    } catch (const QString & msg) {
        throw QString("Не удалось загрузить данные модуля ") + m_loader.id() + ": " + msg;
    }

    // загрузка данных остальных плагинов
    m_container.remove(m_loader.id());
	for (AbstractModule* m : m_container) {
        try {
            QDir dir(epubDir.path()); // QDir каждый раз создается, чтобы гарантировать, что плагин не поменял текущую директорию
            if (dir.cd("seviz") && dir.cd(m->id())) {
                m->load(&dir);
            } else {
                // если папки нет - передаем NULL, чтобы плагин понял, что загружена новая книга
                m->load(nullptr);
            }
        } catch (const QString & msg) {
            throw QString("Не удалось загрузить данные модуля ") + m->id() + ": " + msg;
        }
	}
    m_container.insert(m_loader.id(), &m_loader);
}

void ModuleManager::forEachModule(std::function<void(AbstractModule*)> functor) {
    for (AbstractModule* i : m_container) {
        functor(i);
    }
}

EpubRenderer& ModuleManager::getBookRender() {
    return m_render;
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
    m_container.remove(m_loader.id());
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

void ModuleManager::triggerRerendering(const Position& from, const Position& to) {
    //if (!from.hasSameLevelWith(to)) {
    //    throw std::invalid_argument("if FROM is paragraph then TO must be paragraph, etc");
    //}

    DomChapter styles(m_book->getCurrentChapter());
    for (AbstractModule* m : m_container) {
        QList<Feature*> active = m_enabledFeatures.values(m);
        if (!active.empty()) {
            m->render(from, to, styles, active.toVector());
        }
    }

    m_render.updateChapterView(styles);
}

QList<Feature*> ModuleManager::featureEnabled(const Feature& feature) {
    QList<Feature*> conflicts = getConflictFeaturesFor(feature);

    m_enabledFeatures.insert(feature.owner(), const_cast<Feature*>(&feature));

    if (feature.affectsView()) {
        triggerRerendering(getBook().getCurrentChapter().firstPos(), getBook().getCurrentChapter().lastPos());
    }
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

    m_enabledFeatures.remove(feature.owner(), const_cast<Feature*>(&feature));

    if (feature.affectsView()) {
        triggerRerendering(getBook().getCurrentChapter().firstPos(), getBook().getCurrentChapter().lastPos());
    }
    for (auto& i : m_hotkeys.values(feature)) {
        i->setEnabled(false);
    }
    for (auto& h : m_handlers.values(feature)) {
        m_render.removeHandler(h.first);
        h.second = false;
    }
}

QWidget* ModuleManager::mainWindow() const {
    return m_window;
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

QPair<Position, Position> ModuleManager::selectedTextPos() {
    return m_render.selectedTextPos();
}

Position ModuleManager::mouseHoverElement() {
    return m_render.mouseHoverElement();
}

