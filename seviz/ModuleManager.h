#pragma once
#include <QObject>
#include <QMultiMap>
#include <QList>
#include <QShortcut>
#include <QTemporaryDir>
#include <functional>
#include <memory>
#include "IEngine.h"
#include "AbstractModule.h"
#include "epubrenderer.h"
#include "exceptions.h"
#include "bookloader/bookloader.h"

class MainWindow;

class ModuleManager : public QObject, public IEngine
{
    Q_OBJECT

public:
    ModuleManager(EpubRenderer& render, MainWindow* w);
    ~ModuleManager();

    // методы для ядра
    void bookOpened(Book* book, QTemporaryDir& epubDir, QList<Chapter>& chapters);
    QList<Feature*> featureEnabled(const Feature& feature);
    void featureDisabled(const Feature& feature);

    // для загрузчика книги (BookLoader)
    EpubRenderer& getBookRender();

    // методы для модулей (реализация интерфейса IEngine)
    AbstractModule* getModule(const QString& id, int minVersion = 0) override;
    void forEachModule(std::function<void(AbstractModule*)> functor);
    const Book& getBook() override;

    void triggerRerendering(const Position& from, const Position& to) override;

    void registerHandler(EventType onEvent, ElementType onElements, Button withKey, const Feature& feature, const std::function<void(const Position&)>& slot) override;
    void registerHotkey(const QKeySequence& hotkey, const Feature& feature, const std::function<void()>& slot) override;

    QPair<Position, Position> selectedTextPos() override;
    Position mouseHoverElement() override;
    
private:
    QMap<QString,AbstractModule*> m_container;
    BookLoader m_loader;

    EpubRenderer& m_render;
    MainWindow* m_window;
    Book* m_book = nullptr;
    QMultiMap<const AbstractModule*, Feature*> m_enabledFeatures;

    QMultiMap<Feature, QPair<Handler, bool>> m_handlers;
    QMultiMap<Feature, QShortcut*> m_hotkeys;

    // возвращает функции, которые имеют конфликтующие обработчики или хоткеи с указанной
    QList<Feature*> getConflictFeaturesFor(const Feature& f);

    void destroy();
    std::vector<AbstractModule*> registrar();
};
