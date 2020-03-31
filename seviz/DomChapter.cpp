#include "DomChapter.h"
#include "Book.h"

DomChapter::DomChapter(const Chapter& chapter)
    : QObject(nullptr),
      m_chapter(chapter) {}

DomChapter::~DomChapter() {}

void DomChapter::addStyle(const Position& pos, const QString& css) {
    if (pos.paragraphId() != -1) { // при добавлении стиля к #viewer он почему-то исчезает. в firefox рабоотает нормально
        m_styles[pos] += css;
    }	
}

void DomChapter::addStyleToSpan(const Position& from, const Position& to, const QString& css) {
    if (!(from.hasSameLevelWith(to))) {
        throw std::invalid_argument("from must has same hierarchy level with to: sentence and sentence, ...");
    }
    if (!(from < to)) {
        throw std::invalid_argument("from must be < to");
    }

    Position iter(from);
    iter.setBook(m_chapter.book());

    // TODO refactor

    assert(from.sectionId() != -1);
    assert(from.chapterId() == to.chapterId());

    addStyle(from, css);
    if (from.wordId() != -1) {
        if (iter.hasNextWord()) {
            iter = iter.nextWord();
        }
        if (from.paragraphId() == to.paragraphId() && from.sentenceId() == to.sentenceId()) {
            addStyleToWordCount(iter, to.wordId() - from.wordId() - 1, css);
        } else {
            int idLastWord = m_chapter.book()->getSentence(from).size();
            addStyleToWordCount(iter, idLastWord - from.wordId(), css);
            if (iter.hasNextSentence()) {
                iter = iter.nextSentence();
            }
            addStyleToSentenceSpan(iter, from, to, css);
            iter = iter.firstWord();
            addStyleToWordCount(iter, to.wordId() - 1, css);
        }
    } else if (from.sentenceId() != -1) {
        if (iter.hasNextSentence()) {
            iter = iter.nextSentence();
        }
        addStyleToSentenceSpan(iter, from, to, css);
    } else if (from.paragraphId() != -1) {
        if (iter.hasNextParagraph()) {
            iter = iter.nextParagraph();
        }
        addStyleToParagraphCount(iter, to.sentenceId() - from.sentenceId() - 1, css);
    }
    addStyle(to, css);
    
}

void DomChapter::addTailContent(const Position& tailPos, const QString& tailText) {
    if (m_tails.contains(tailPos)) {
        throw std::runtime_error("can't set tail content twice");
    } else {
        m_tails.insert(tailPos, tailText);
    }
}

void DomChapter::addTailContent(const Position& pos, TailPosition where, const QString& tailText) {
    assert(pos.tail() == NOT_TAIL);
    Position tailPos = Position(pos.chapterId(), pos.sectionId(), pos.paragraphId(), pos.sentenceId(), pos.wordId(), where);
    addTailContent(tailPos, tailText);
}

void DomChapter::addTooltip(const Position& pos, const QString& text) {
    if (m_tooltips.contains(pos)) {
        throw std::runtime_error("can't set tooltip for element twice");
    } else {
        m_tooltips.insert(pos, text);
    }
}

void DomChapter::addTooltip(const Position& pos, TailPosition where, const QString& tooltipText) {
    assert(pos.tail() == NOT_TAIL);
    Position tailPos = Position(pos.chapterId(), pos.sectionId(), pos.paragraphId(), pos.sentenceId(), pos.wordId(), where);
    addTooltip(tailPos, tooltipText);
}

void DomChapter::addStyleToSentenceSpan(Position& iter, const Position& from, const Position& to, const QString& css) {
    if (from.paragraphId() == to.paragraphId()) {
        addStyleToSentenceCount(iter, to.sentenceId() - from.sentenceId() - 1, css);
    } else {
        int idLastSentInPar = m_chapter.book()->getParagraph(from).size();
        addStyleToSentenceCount(iter, idLastSentInPar - from.sentenceId(), css);
        iter = iter.nextParagraph();
        addStyleToParagraphCount(iter, to.paragraphId() - from.paragraphId() - 1, css);
        iter = iter.firstSentence();
        addStyleToSentenceCount(iter, to.sentenceId() - 1, css);
        if (iter.hasNextSentence()) {
            iter = iter.nextSentence();
        }
    }
}

void DomChapter::addStyleToWordCount(Position& first, int n, const QString& css) {
    for (int i = 0; i < n - 1; ++i) {
        addStyle(first, css);
        first = first.nextWord();
    }
    if (n > 0) {
        addStyle(first, css);
    }
}

void DomChapter::addStyleToSentenceCount(Position& first, int n, const QString& css) {
    for (int i = 0; i < n - 1; ++i) {
        addStyle(first, css);
        first = first.nextSentence();
    }
    if (n > 0) {
        addStyle(first, css);
    }
}

void DomChapter::addStyleToParagraphCount(Position& first, int n, const QString& css) {
    for (int i = 0; i < n - 1; ++i) {
        addStyle(first, css);
        first = first.nextParagraph();
    }
    if (n > 0) {
        addStyle(first, css);
    }
}