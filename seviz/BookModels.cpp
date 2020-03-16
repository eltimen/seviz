#include "BookModels.h"
#include "Book.h"

Position::Position(int idChapter, int idSection, int idParagraph, int idSentence, int idWord, const Book* book) :
    m_idChapter(idChapter),
    m_idSection(idSection),
    m_idParagraph(idParagraph),
    m_idSentence(idSentence),
    m_idWord(idWord),
    m_book(book) {
    if (!(idChapter >= 1) ||
        !(idSection == -1 || idSection >= 1) ||
        !(idParagraph == -1 || idParagraph >= 1) ||
        !(idSentence == -1 || idSentence >= 1) ||
        !(idWord == -1 || idWord >= 1))

        throw std::invalid_argument("id must be >=1");
}

bool Position::operator<(const Position& o) const {
    // TODO refactor, unit tests
    if (m_idChapter < o.m_idChapter) {
        return true;
    } else if (m_idSection < o.m_idSection) {
        return true;
    } else if (m_idParagraph < o.m_idParagraph) {
        return true;
    } else if (m_idSentence < o.m_idSentence) {
        return true;
    } else if (m_idWord < o.m_idWord) {
        return true;
    } else {
        return false;
    }
}

bool Position::operator==(const Position& o) const {
    return m_idChapter == o.m_idChapter &&
        m_idSection == o.m_idSection &&
        m_idParagraph == o.m_idParagraph &&
        m_idSentence == o.m_idSentence &&
        m_idWord == o.m_idWord;
}

bool Position::operator<=(const Position& o) const {
    return *this < o || *this == o;
}

bool Position::hasSameLevelWith(const Position& o) const {
    return (m_idChapter == -1) == (o.m_idChapter == -1) &&
        (m_idSection == -1) == (o.m_idSection == -1) &&
        (m_idParagraph == -1) == (o.m_idParagraph == -1) &&
        (m_idSentence == -1) == (o.m_idSentence == -1) &&
        (m_idWord == -1) == (o.m_idWord == -1);
}

bool Position::hasElement(ElementType type) {
    if (type == ElementType::PARAGRAPH) {
        return paragraphId() != -1;
    } else if (type == ElementType::SENTENCE) {
        return sentenceId() != -1;
    } else if (type == ElementType::WORD) {
        return wordId() != -1;
    } else {
        return true;
    }
}

Position Position::prevChapter() const {
    int val = m_idChapter-1;
    if (val < 1) {
        throw std::range_error("chapter not exist");
    }
    return Position(val, -1, -1, -1, -1, m_book);
}

Position Position::nextChapter() const {
    int val = m_idChapter + 1;
    if (val > m_book->chapters().size()) {
        throw std::range_error("chapter not exist");
    }
    return Position(val, -1, -1, -1, -1, m_book);
}

Position Position::prevSection() const {
    int val = m_idSection - 1;
    if (val < 1) {
        throw std::range_error("section not exist");
    }
    return Position(m_idChapter, val, -1, -1, -1, m_book);
}

Position Position::nextSection() const {
    int val = m_idSection + 1;
    if (val < 1 || !hasNextSection()) {
        throw std::range_error("section not exist");
    }
    return Position(m_idChapter, val, -1, -1, -1, m_book);
}

Position Position::prevParagraph() const {
    int val = m_idParagraph - 1;
    if (val < 1) {
        throw std::range_error("paragraph not exist");
    }
    return Position(m_idChapter, m_idSection, val, -1, -1, m_book);
}

Position Position::nextParagraph() const {
    int val = m_idParagraph + 1;
    if (val < 1 || !hasNextParagraph()) {
        throw std::range_error("paragraph not exist");
    }
    return Position(m_idChapter, m_idSection, val, -1, -1, m_book);
}

Position Position::prevSentence() const {
    int val = m_idSentence - 1;
    if (val < 1) {
        throw std::range_error("sentence not exist");
    }
    return Position(m_idChapter, m_idSection, m_idParagraph, val, -1, m_book);
}

Position Position::nextSentence() const {
    int val = m_idSentence + 1;
    if (val < 1 || !hasNextSentence()) {
        throw std::range_error("sentence not exist");
    }
    return Position(m_idChapter, m_idSection, m_idParagraph, val, -1, m_book);
}

Position Position::prevWord() const {
    int val = m_idWord - 1;
    if (val < 1) {
        throw std::range_error("word not exist");
    }
    return Position(m_idChapter, m_idSection, m_idParagraph, m_idSentence, val, m_book);
}

Position Position::nextWord() const {
    int val = m_idWord + 1;
    if (val < 1 || !hasNextWord()) {
        throw std::range_error("word not exist");
    }
    return Position(m_idChapter, m_idSection, m_idParagraph, m_idSentence, val, m_book);
}

bool Position::hasNextChapter() const {
    return m_idChapter + 1 < m_book->chapters().size();
}

bool Position::hasNextSection() const {
    int val = m_idSection + 1;
    return val > 0 && val <= m_book->chapters()[m_idChapter - 1].sections.size();
}

bool Position::hasNextParagraph() const {
    int val = m_idParagraph + 1;
    return val > 0 && val <= m_book->getSection(*this).size();
}

bool Position::hasNextSentence() const {
    int val = m_idSentence + 1;
    return val > 0 && val <= m_book->getParagraph(*this).size();
}

bool Position::hasNextWord() const {
    int val = m_idWord + 1;
    return val > 0 && val <= m_book->getSentence(*this).size();
}

Position Position::firstSentence() const {
    return Position(m_idChapter,
        m_idSection > 0 ? m_idSection : 1,
        m_idParagraph > 0 ? m_idParagraph : 1,
        1,
        -1,
        m_book);
}

Position Position::firstWord() const {
    return Position(m_idChapter, 
        m_idSection > 0 ? m_idSection : 1, 
        m_idParagraph > 0 ? m_idParagraph : 1,
        m_idSentence > 0 ? m_idSentence : 1,
        1,
        m_book);
}

QString Position::cssSelector() const {
    QString sel = "#viewer ";

    if (m_idParagraph > 0)
        sel += QStringLiteral("> p:nth-of-type(%1) ").arg(m_idParagraph);
    if (m_idSentence > 0)
        sel += QStringLiteral("> sentence:nth-of-type(%1) ").arg(m_idSentence);
    if (m_idWord > 0)
        sel += QStringLiteral("> word:nth-of-type(%1) ").arg(m_idWord);

    return sel;
}
