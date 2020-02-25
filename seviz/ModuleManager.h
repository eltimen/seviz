#pragma once
#include <QObject>
#include <QMultiMap>
#include <QList>
#include <QShortcut>
#include <functional>
#include "AbstractModule.h"
#include "EventModels.h"
#include "BookModels.h"
#include "epubrenderer.h"
#include "exceptions.h"

class MainWindow;
class Book;

// макросы для передачи слотов как колбеков в методы register...
#define VOIDSLOT(x) [this](){x();}

class ModuleManager : public QObject
{
    Q_OBJECT

public:
    ModuleManager(EpubRenderer& render, MainWindow* w);
    ~ModuleManager();

    void bookOpened(Book* book);
    QList<Feature*> featureEnabled(const Feature& feature);
    void featureDisabled(const Feature& feature);
    void forEachModule(std::function<void(AbstractModule*)> functor);

    // методы для модулей
    AbstractModule* getModule(const QString& id, int minVersion = 0);
    const Book& getBook();

    void triggerRerendering(const Position& from, const Position& to);

    void registerHandler(EventType onEvent, ElementType onElements, Button withKey, const Feature& feature, const std::function<void(const Position&)>& slot);
    void registerHotkey(const QKeySequence& hotkey, const Feature& feature, const std::function<void()>& slot);

    QPair<Position, Position> selectedTextPos();
    Position mouseHoverElement(ElementType elem);
    
private:
    QMap<QString,AbstractModule*> m_container;
    EpubRenderer& m_render;
    MainWindow* m_window;
    Book* m_book = nullptr;

    QMultiMap<Feature, QPair<Handler, bool>> m_handlers;
    QMultiMap<Feature, QShortcut*> m_hotkeys;

    // возвращает функции, которые имеют конфликтующие обработчики или хоткеи с указанной
    QList<Feature*> getConflictFeaturesFor(const Feature& f);

    void destroy();
    std::vector<AbstractModule*> registrar();
};
