#pragma once

#include <QString>
#include <functional>
#include "Book.h"
#include "BookModels.h"
#include "EventModels.h"
#include "DomChapter.h"
#include "Feature.h"
#include "exceptions.h"

// макросы для передачи слотов-методов класса как колбеков в методы register...
// пример использования: SLOTPOS(MyOwnPlugin::onSentenceClicked)
// TODO избавиться от макросов, сделать на шаблонах наподобие QObject::connect
#define SLOTVOID(x) [this](){x();}
#define SLOTPOS(x) [this](const Position& pos){x(pos);}

class IEngine {
public:
    virtual void forEachModule(std::function<void(ISevizPlugin*)> functor) = 0;
    virtual ISevizPlugin* getPlugin(const QString& id, int minVersion = 0) = 0;
    virtual const Book& getBook() = 0;

    virtual void triggerRerendering(const Position& from, const Position& to) = 0;

    virtual void registerHandler(EventType onEvent, ElementType onElements, Button withKey, const Feature& feature, const std::function<void(const Position&)>& slot) = 0;
    virtual void registerHotkey(const QKeySequence& hotkey, const Feature& feature, const std::function<void()>& slot) = 0;

    virtual QPair<Position, Position> selectedTextPos() = 0;
    virtual Position mouseHoverElement() = 0;
};

