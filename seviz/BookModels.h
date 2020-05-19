#pragma once

#include <QList>
#include <QString>
#include "EventModels.h"

class Book;
class Chapter;
class Section;
class Paragraph;
class Sentence;
class Word; // знаки пунктуации и слова

class Scene;
class Fragment;

enum TailPosition {
    NOT_TAIL = 0,
    TOPLEFT,
    TOPRIGHT,
    BOTTOMLEFT,
    BOTTOMRIGHT
};

class Position {
public:
    Position(int idChapter, int idSection = -1, int idParagraph = -1, int idSentence = -1, int idWord = -1, TailPosition tail = NOT_TAIL);
    Position() : Position(1) {}

    bool operator<(const Position& o) const;
    bool operator==(const Position& o) const;
    bool operator<=(const Position& o) const;

    bool hasSameLevelWith(const Position& o) const;

    bool isEmpty() const { return m_idChapter == -1; }

    int chapterIndex() const { return m_idChapter - 1; }
    int sectionIndex() const { return m_idSection - 1; }
    int paragraphIndex() const { return m_idParagraph - 1; }
    int sentenceIndex() const { return m_idSentence - 1; }
    int wordIndex() const { return m_idWord - 1; }
    int chapterId() const { return m_idChapter; }
    int sectionId() const { return m_idSection; }
    int paragraphId() const { return m_idParagraph; }
    int sentenceId() const { return m_idSentence; }
    int wordId() const { return m_idWord; }
    TailPosition tail() const { return m_tail; }

    Position trimmedTo(ElementType type) const;

    bool hasElement(ElementType type);

    static void setBook(const Book* book) { m_book = book; }

    // !!! для функций-итераторов предварительно нужно задать книгу через setBook
    Position prevChapter() const;
    Position prevSection() const;
    Position prevParagraph() const;
    Position prevSentence() const; 
    Position prevWord() const;

    bool hasPrevChapter() const;
    bool hasPrevSection() const;
    bool hasPrevParagraph() const;
    bool hasPrevSentence() const;
    bool hasPrevWord() const;

    Position nextChapter() const;
    Position nextSection() const;    
    Position nextParagraph() const;
    Position nextSentence() const;
    Position nextWord() const;

    bool hasNextChapter() const;
    bool hasNextSection() const;
    bool hasNextParagraph() const;
    bool hasNextSentence() const;
    bool hasNextWord() const;
    
    Position firstSentence() const;
    Position firstWord() const;

    QString cssSelector() const;

private:
    int m_idChapter;
    int m_idSection;
    int m_idParagraph;
    int m_idSentence;
    int m_idWord;
    TailPosition m_tail;

    static const Book* m_book;
};

class Section : public QList<Paragraph> {
public:
    //Section(int id, const QString& name, const QList<Paragraph>& content) : 
    //	m_id(id), m_name(name), QList<Paragraph>(content) {}
    Section(int id, const QList<Paragraph>& content) :
        m_id(id), QList<Paragraph>(content) {}

    int id() const { return m_id; }
    //const QString& name() const { return m_name; }
private:
    int m_id;
    //QString m_name;
};

class Paragraph : public QList<Sentence> {
public:
    Paragraph(int id, const QList<Sentence>& content) :
        m_id(id), QList<Sentence>(content) {}
    int id() const { return m_id; }
private:
    int m_id;
};

class Sentence : public QList<Word> {
    int m_id;
public:
    Sentence() : Sentence(-1, {}) {};
    Sentence(int id, const QList<Word>& content) :
        m_id(id), QList<Word>(content) {}

    int id() const { return m_id; }
};

class Word {
    int m_id;
    QString m_text;

public:
    Word(int id, const QString& text) :
        m_id(id), m_text(text) {
    }

    bool operator< (const Word& o) const {
        return m_id < o.m_id;
    }

    int id() const { return m_id; }
    const QString& text() const { return m_text; }

    bool isPunct() const { return m_text.length() == 1 && !QRegExp("\\w+").exactMatch(m_text); }
};

class Chapter {
public:
    Chapter(const Book* book, int id, const QString& name) :
        m_id(id), m_name(name), m_book(book) {}

    const Book* book() const { return m_book; }
    int id() const { return m_id; }
    const QString& name() const { return m_name; } 

    bool isInitialized() const { return sections.size() > 0; }

    Position firstPos() const { 
        const Section& firstSect = sections.first();
        int firstSectId = firstSect.id();
        int firstParId = -1;
        int firstSentId = -1;
        int firstWordId = -1;

        if (!firstSect.empty()) {
            const Paragraph& firstPar = firstSect.first();
            firstParId = firstPar.id();
            if (!firstPar.empty()) {
                const Sentence& firstSent = firstPar.first();
                firstSentId = firstSent.id();
                if (!firstSent.empty()) {
                    const Word& firstWord = firstSent.first();
                    firstWordId = firstWord.id();
                }
            }
        }

        return Position(m_id, firstSectId, firstParId, firstSentId, firstWordId);
    }

    Position lastPos() const {
        const Section& lastSect = sections.last();
        int lastSectId = lastSect.id();
        int lastParId = -1;
        int lastSentId = -1;
        int lastWordId = -1;

        if (!lastSect.empty()) {
            const Paragraph& lastPar = lastSect.last();
            lastParId = lastPar.id();
            if (!lastPar.empty()) {
                const Sentence& lastSent = lastPar.last();
                lastSentId = lastSent.id();
                if (!lastSent.empty()) {
                    const Word& lastWord = lastSent.last();
                    lastWordId = lastWord.id();
                }
            }
        }

        return Position(m_id, lastSectId, lastParId, lastSentId, lastWordId);
    }

    QList<Section> sections;
    QList<Scene> scenes;

private:
    const Book* m_book;
    int m_id;
    QString m_name;
};

class Scene : public QList<Fragment> {
    int id;
    int idParagraphStart;
    int idParagraphEnd;
};

class Fragment {
    int id;
    QPair<Position, Position> positions; // границы по предложениям
};

