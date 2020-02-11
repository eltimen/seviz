#pragma once
#include <QObject>
#include <QHash>
#include <QList>
#include <memory>
#include <functional>
#include "AbstractModule.h"
#include "BookModels.h"
#include "epubrenderer.h"
#include "exceptions.h"

class Book;

class ModuleManager : public QObject
{
    Q_OBJECT

public:
    ModuleManager(EpubRenderer& render);
    ~ModuleManager();

    void bookOpened(Book* book);
    void featureEnabled(const Feature& feature);
    void featureDisabled(const Feature& feature);
    void forEachModule(std::function<void(AbstractModule*)> functor);

    // методы для модулей
    AbstractModule* getModule(const QString& id, int minVersion);
    const Book& getBook();
    void triggerRerendering(const Position& from, const Position& to);
    //void registerHandler(AbstractModule& module, enum ElementType elem, enum EventType onEvent, const Feature& feature, std::function<void()>& slot);
    //void registerHotkey(AbstractModule& module, const QKeySequence& hotkey, const Feature& feature, std::function<void()>& slot);
    QPair<Position, Position> selectedTextPos();
    Position mouseHoverElement(enum ElementType elem);
    
private:
    QHash<QString,AbstractModule*> m_container;
    EpubRenderer& m_render;
    Book* m_book = nullptr;

    void destroy();
};
