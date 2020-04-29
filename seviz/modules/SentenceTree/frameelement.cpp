#include "frameelement.h"
#include "framenet.h"

bool FrameElement::isWord() const {
    return !m_subFrame;
}

bool FrameElement::isFrame() const {
    return m_subFrame;
}

WordRange FrameElement::range() const {
    if (m_subFrame) {
        return m_subFrame->range();
    } else {
        return WordRange(m_words.begin()->id(), (m_words.end() - 1)->id());
    }
}

Frame* FrameElement::childFrame() const {
    return m_subFrame;
}

QList<Word> FrameElement::words() const {
    return m_words;
}
