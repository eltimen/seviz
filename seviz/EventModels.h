#pragma once

class Position;

enum Button {
    NONE = 0,
    CTRL = 1,
    SHIFT = 2,
    ALT = 4
};

enum class EventType {
    MOUSE_LCLICK,
    MOUSE_RCLICK,
    MOUSE_DBLCLICK,
    //MOUSE_HOVER
    //MOUSE_LEFT_UP,
    //MOUSE_LEFT_DOWN,
    //MOUSE_MOVE
};

enum class ElementType {
    PARAGRAPH,
    SENTENCE,
    WORD
};

struct Handler {
    EventType onEvent;
    ElementType onElements;
    Button modifierKey;
    std::function<void(const Position&)> slot;

    Handler(EventType onEvent, ElementType onElements, Button modifierKey, const std::function<void(const Position&)>& slot) :
        onEvent(onEvent), onElements(onElements), modifierKey(modifierKey), slot(slot)
    {}

    bool operator==(const Handler& other) const {
        return onEvent == other.onEvent &&
            onElements == other.onElements &&
            modifierKey == other.modifierKey;
    };
};