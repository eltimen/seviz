#pragma once

#include <QList>
#include <QString>

#include "EventModels.h"

class Chapter;
class Section;
class Paragraph;
class Sentence;
class Word; // знаки пунктуации - пусть пока будут тоже словами

class Scene;
class Fragment;

class Position {
public:
    Position(int idChapter, int idSection = -1, int idParagraph = -1, int idSentence = -1, int idWord = -1) :
        m_idChapter(idChapter), 
        m_idSection(idSection), 
        m_idParagraph(idParagraph), 
        m_idSentence(idSentence), 
        m_idWord(idWord) 
    {
        if (!(idChapter == -1   || idChapter >= 1   )||
            !(idSection == -1   || idSection >= 1   )||
            !(idParagraph == -1 || idParagraph >= 1 )||
            !(idSentence == -1  || idSentence >= 1  )||
            !(idWord == -1      || idWord >= 1	    ))

            throw std::invalid_argument("id must be >=1");
    }

    Position() : Position(-1) {}

    bool operator<(const Position& o) const {
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

    bool hasSameLevelWith(const Position& o) const {
        return (m_idChapter == -1)   == (o.m_idChapter == -1)   &&
               (m_idSection == -1)   == (o.m_idSection == -1)   &&
               (m_idParagraph == -1) == (o.m_idParagraph == -1) &&
               (m_idSentence == -1)  == (o.m_idSentence == -1)  &&
               (m_idWord == -1)      == (o.m_idWord == -1);
    }

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

    bool hasElement(ElementType type) {
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

    QString cssSelector() const { 
        QString sel = "#viewer ";

        if (m_idParagraph > 0)
            sel += QStringLiteral("> p:nth-child(%1) ").arg(2+m_idParagraph);
        if (m_idSentence > 0)
            sel += QStringLiteral("> sentence:nth-child(%1) ").arg(m_idSentence);
        if (m_idWord > 0)
            sel += QStringLiteral("> word:nth-child(%1) ").arg(m_idWord);

        return sel; 
    }

private:
    int m_idChapter;
    int m_idSection;
    int m_idParagraph;
    int m_idSentence;
    int m_idWord;
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
};

class Chapter {
public:
    Chapter(int id, const QString& name) :
        m_id(id), m_name(name) {}

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

