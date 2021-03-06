#include "BookModels.h"
#include "exceptions.h"
#include "Book.h"

const Book* Position::m_book = nullptr;

Position::Position(int idChapter, int idSection, int idParagraph, int idSentence, int idWord, TailPosition tail)
    : m_idChapter(idChapter), 
      m_idSection(idSection), 
      m_idParagraph(idParagraph), 
      m_idSentence(idSentence), 
      m_idWord(idWord),
      m_tail(tail) 
{
    if (!(idChapter >= 1) ||
        !(idSection == -1 || idSection >= 1) ||
        !(idParagraph == -1 || idParagraph >= 1) ||
        !(idSentence == -1 || idSentence >= 1) ||
        !(idWord == -1 || idWord >= 1))

        throw std::invalid_argument("id must be >=1");
} 

bool Position::operator<(const Position& o) const {
    return std::tie(m_idChapter, m_idSection, m_idParagraph, m_idSentence, m_idWord, m_tail)
         < std::tie(o.m_idChapter, o.m_idSection, o.m_idParagraph, o.m_idSentence, o.m_idWord, o.m_tail);
}

bool Position::operator==(const Position& o) const {
    return m_idChapter == o.m_idChapter &&
        m_idSection == o.m_idSection &&
        m_idParagraph == o.m_idParagraph &&
        m_idSentence == o.m_idSentence &&
        m_idWord == o.m_idWord &&
        m_tail == o.m_tail;
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

Position Position::trimmedTo(ElementType type) const {
    Position temp(*this);
    if (type == ElementType::PARAGRAPH) {
        temp.m_idSentence = -1;
        temp.m_idWord = -1;
    } else if (type == ElementType::SENTENCE) {
        temp.m_idWord = -1;
    }
    return temp;
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
    return Position(val, -1, -1, -1, -1, NOT_TAIL);
}

Position Position::nextChapter() const {
    int val = m_idChapter + 1;
    if (val > m_book->chapters().size()) {
        throw std::range_error("chapter not exist");
    }
    return Position(val, -1, -1, -1, -1, NOT_TAIL);
}

Position Position::prevSection() const {
    int val = m_idSection - 1;
    if (val < 1) {
        throw std::range_error("section not exist");
    }
    return Position(m_idChapter, val, -1, -1, -1, NOT_TAIL);
}

Position Position::nextSection() const {
    int val = m_idSection + 1;
    if (val < 1 || !hasNextSection()) {
        throw std::range_error("section not exist");
    }
    return Position(m_idChapter, val, -1, -1, -1, NOT_TAIL);
}

Position Position::prevParagraph() const {
    int val = m_idParagraph - 1;
    if (val < 1) {
        throw std::range_error("paragraph not exist");
    }
    return Position(m_idChapter, m_idSection, val, -1, -1, NOT_TAIL);
}

Position Position::nextParagraph() const {
    int val = m_idParagraph + 1;
    if (val < 1 || !hasNextParagraph()) {
        throw std::range_error("paragraph not exist");
    }
    return Position(m_idChapter, m_idSection, val, -1, -1, NOT_TAIL);
}

Position Position::prevSentence() const {
    int val = m_idSentence - 1;
    if (val < 1) {
        throw std::range_error("sentence not exist");
    }
    return Position(m_idChapter, m_idSection, m_idParagraph, val, -1, NOT_TAIL);
}

Position Position::nextSentence() const {
    int val = m_idSentence + 1;
    if (val < 1 || !hasNextSentence()) {
        throw std::range_error("sentence not exist");
    }
    return Position(m_idChapter, m_idSection, m_idParagraph, val, -1, NOT_TAIL);
}

Position Position::prevWord() const {
    int val = m_idWord - 1;
    if (val < 1) {
        throw std::range_error("word not exist");
    }
    return Position(m_idChapter, m_idSection, m_idParagraph, m_idSentence, val, NOT_TAIL);
}

bool Position::hasPrevChapter() const {
    return m_idChapter - 1 >= 1;
}

bool Position::hasPrevSection() const {
    return m_idSection - 1 >= 1;
}

bool Position::hasPrevParagraph() const {
    return m_idParagraph - 1 >= 1;
}

bool Position::hasPrevSentence() const {
    return m_idSentence - 1 >= 1;
}

bool Position::hasPrevWord() const {
    return m_idWord - 1 >= 1;
}

Position Position::nextWord() const {
    int val = m_idWord + 1;
    if (val < 1 || !hasNextWord()) {
        throw std::range_error("word not exist");
    }
    return Position(m_idChapter, m_idSection, m_idParagraph, m_idSentence, val, NOT_TAIL);
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
        NOT_TAIL);
}

Position Position::lastSentence() const {
    return Position(m_idChapter,
                    m_idSection > 0 ? m_idSection : 1,
                    m_idParagraph > 0 ? m_idParagraph : 1,
                    m_book->getParagraph(*this).size(),
                    -1,
                    NOT_TAIL);
}

Position Position::firstWord() const {
    return Position(m_idChapter, 
        m_idSection > 0 ? m_idSection : 1, 
        m_idParagraph > 0 ? m_idParagraph : 1,
        m_idSentence > 0 ? m_idSentence : 1,
        1,
        NOT_TAIL);
}

QString Position::cssSelector() const {
    QString sel;

    if (m_tail == NOT_TAIL) {
        sel = "#viewer ";
        if (m_idParagraph > 0)
            sel += QStringLiteral("> p:nth-of-type(%1) ").arg(m_idParagraph);
        if (m_idSentence > 0)
            sel += QStringLiteral("> sentence:nth-of-type(%1) ").arg(m_idSentence);
        if (m_idWord > 0)
            sel += QStringLiteral("> word:nth-of-type(%1) ").arg(m_idWord);
    } else {
        QString tailNumber = QString::number(m_idParagraph);
        if (m_idSentence > 0)
            tailNumber += QStringLiteral("_%1").arg(m_idSentence);
        if (m_idWord > 0)
            tailNumber += QStringLiteral("_%1").arg(m_idWord);

        if (m_tail == TOPLEFT) {
            sel += QStringLiteral("#shl%1 ").arg(tailNumber);
        } else if (m_tail == TOPRIGHT) {
            sel += QStringLiteral("#shr%1 ").arg(tailNumber);
        } else if (m_tail == BOTTOMLEFT) {
            sel += QStringLiteral("#sdl%1 ").arg(tailNumber);
        } else if (m_tail == BOTTOMRIGHT) {
            sel += QStringLiteral("#sdr%1 ").arg(tailNumber);
        }
    }

    return sel;
}


const QStringList Word::PosTagsStr = 
{
    "?",
    "CC", "CD", "DT", "EX", "FW", "IN", "JJ", "JJR", "JJS", "LS", "MD", "NN", "NNS", "NNP", "NNPS",
    "PDT", "POS", "PRP", "PRP$", "PP$", "RB", "RBR", "RBS", "RP", "SYM", "TO", "UH", "VB", "VBD", "VBG", "VBN",
    "VBP", "VBZ", "WDT", "WP", "WP$", "WRB",
    "#", "$", ".", ",", ":", "(", ")", "\"", "‘", "“", "’", "”"
};


void Word::setPOS(const QString& posTag) {
    QString tag = posTag.trimmed();
    if (!PosTagsStr.contains(tag)) {
        throw std::invalid_argument("invalid pos tag");
    }
    m_POS = tag;
}

QString Word::getPosTagColor(const QString& posTag){
    // taken from https://github.com/stanfordnlp/CoreNLP/blob/master/src/edu/stanford/nlp/pipeline/demo/corenlp-brat.js
    if (posTag.startsWith("N")) {
        return "#A4BCED";
    } else if (posTag.startsWith("V") || posTag.startsWith("M")) {
        return "#ADF6A2";
    } else if (posTag.startsWith("P")) {
        return "#CCDAF6";
    } else if (posTag.startsWith("I")) {
        return "#FFE8BE";
    } else if (posTag.startsWith("R") || posTag.startsWith("W")) {
        return "#FFFDA8";
    } else if (posTag.startsWith("D") || posTag == "CD") {
        return "#CCADF6";
    } else if (posTag.startsWith("J")) {
        return "#FFFDA8";
    } else if (posTag.startsWith("T")) {
        return "#FFE8BE";
    } else if (posTag.startsWith("E") || posTag.startsWith("S")) {
        return "#E4CBF6";
    } else if (posTag.startsWith("CC")) {
        return "#FFFFFF";
    } else if (posTag == "LS" || posTag == "FW") {
        return "#FFFFFF";
    } else {
        return "#E3E3E3";
    }
    return QString();
}

QStringList Sentence::toStringList() {
    QStringList tokens;
    for (const auto& w : *this) {
        tokens << w.text();
    }
    return tokens;
}

uint qHash(const Word &w, uint seed) noexcept {
    return qHash(w.text(), seed);
}
