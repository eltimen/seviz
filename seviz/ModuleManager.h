#pragma once
#include <QObject>
#include <QMultiMap>
#include <QList>
#include <QShortcut>
#include <functional>
#include "AbstractModule.h"
#include "BookModels.h"
#include "epubrenderer.h"
#include "exceptions.h"

class MainWindow;
class Book;

class ModuleManager : public QObject
{
    Q_OBJECT

public:
    ModuleManager(EpubRenderer& render, MainWindow* w);
    ~ModuleManager();

    void bookOpened(Book* book);
    void featureEnabled(const Feature& feature);
    void featureDisabled(const Feature& feature);
    void forEachModule(std::function<void(AbstractModule*)> functor);

    // методы для модулей
    AbstractModule* getModule(const QString& id);
    AbstractModule* getModule(const QString& id, int minVersion);
    const Book& getBook();
    void triggerRerendering(const Position& from, const Position& to);
    //void registerHandler(enum ElementType elem, enum EventType onEvent, const Feature& feature, std::function<void()>& slot);
    void registerHotkey(const QKeySequence& hotkey, const Feature& feature, const std::function<void()>& slot);
    QPair<Position, Position> selectedTextPos();
    Position mouseHoverElement(enum ElementType elem);
    
private:
    QMap<QString,AbstractModule*> m_container;
    EpubRenderer& m_render;
    MainWindow* m_window;
    Book* m_book = nullptr;

    QMultiMap<Feature, QShortcut*> m_hotkeys;

    void destroy();
    std::vector<AbstractModule*> registrar();
};
