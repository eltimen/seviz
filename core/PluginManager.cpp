#include "PluginManager.h"
#include <tuple>
#include <iterator>
#include <QMessageBox>
#include "DomChapter.h"
#include "mainwindow.h"

PluginManager::PluginManager(EpubRenderer& render, MainWindow* w)
    : m_render(render),
      m_window(w),
      m_loader(this) {
    QDir pluginsDir( "./plugins" );
    for (const QString& pluginName : pluginsDir.entryList(QDir::Files)) {
        qDebug() << "seviz: trying to load plugin " << pluginName;
        QPluginLoader* loader = new QPluginLoader(pluginsDir.absoluteFilePath(pluginName));
        ISevizPlugin* plugin = nullptr;
        if(loader->load() && (plugin=qobject_cast<ISevizPlugin*>(loader->instance()))) {
            if (!m_container.contains(plugin->id())) {
                plugin->init(this);
                m_container.insert(plugin->id(), qMakePair(loader, plugin));
            } else {
                QMessageBox::warning(w, "Ошибка", "Обнаружен дублирующийся плагин " + plugin->id());
                loader->unload();
                plugin = nullptr;
            }
        } else {
            QMessageBox::warning(w, "Ошибка", "Не удалось загрузить плагин: " + loader->errorString());
        }
    }
}

PluginManager::~PluginManager() {
    // smart pointers std::unique_ptr or QScopedPointer can't be stored inside Qt containers
    m_container.remove(m_loader.id());
    for (auto& val : m_container) {
        val.first->unload();
        val.second = nullptr;
    }
    m_container.clear();
}

void PluginManager::bookOpened(Book* book, QTemporaryDir& epubDir, QList<Chapter>& chapters) {
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
    for (auto& val : m_container) {
        ISevizPlugin* m = val.second;
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
    m_container.insert(m_loader.id(), qMakePair(nullptr, &m_loader));
}

void PluginManager::forEachModule(std::function<void(ISevizPlugin*)> functor) {
    for (auto& i : m_container) {
        functor(i.second);
    }
}

EpubRenderer& PluginManager::getBookRender() {
    return m_render;
}

QList<Feature*> PluginManager::getConflictFeaturesFor(const Feature& f) {
    // для каждого хоткея из f
    for (decltype(m_hotkeys)::iterator it = m_hotkeys.begin(); it != m_hotkeys.end(); ++it) {
        // ищем его среди относящихся к другой функции
        // TODO
    }

    // TODO аналогично для обработчиков

    return {};
}

ISevizPlugin* PluginManager::getPlugin(const QString& id, int minVersion) {
    ISevizPlugin* m = m_container.value(id, qMakePair(nullptr, nullptr)).second;
    return m->version() >= minVersion ? m : nullptr;
}

const Book& PluginManager::getBook() {
    return *m_book;
}

void PluginManager::triggerRerendering(const Position& from, const Position& to) {
    //if (!from.hasSameLevelWith(to)) {
    //    throw std::invalid_argument("if FROM is paragraph then TO must be paragraph, etc");
    //}

    DomChapter styles(m_book->getCurrentChapter());
    for (auto& val : m_container) {
        ISevizPlugin* m = val.second;
        QList<Feature*> active = m_enabledFeatures.values(m);
        if (!active.empty()) {
            m->render(from, to, styles, active.toVector());
        }
    }

    m_render.updateChapterView(styles);
}

QList<Feature*> PluginManager::featureEnabled(const Feature& feature) {
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

void PluginManager::featureDisabled(const Feature& feature) {

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

QWidget* PluginManager::mainWindow() const {
    return m_window;
}

void PluginManager::registerHandler(EventType onEvent, ElementType onElements, Button withKey, const Feature& feature, const std::function<void(const Position&)>& slot) {
    Handler h(onEvent, onElements, withKey, slot);

    // TODO проверка

    m_handlers.insert(feature, qMakePair(h, false));
}

void PluginManager::registerHotkey(const QKeySequence& hotkey, const Feature& feature, const std::function<void()>& slot) {
    // TODO проверить, что модуль не регистрирует один и тот же хоткей дважды

    QShortcut* sh = new QShortcut(hotkey, m_window, nullptr, nullptr, Qt::ApplicationShortcut);
    sh->setEnabled(false);
    connect(sh, &QShortcut::activated, slot);
    m_hotkeys.insert(feature, sh);
}

QPair<Position, Position> PluginManager::selectedTextPos() {
    return m_render.selectedTextPos();
}

Position PluginManager::mouseHoverElement() {
    return m_render.mouseHoverElement();
}

