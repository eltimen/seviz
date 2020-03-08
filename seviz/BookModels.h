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
        return m_idChapter   < o.m_idChapter &&
               m_idSection   < o.m_idSection &&
               m_idSection   < o.m_idParagraph &&
               m_idSentence  < o.m_idSentence &&
               m_idWord      < o.m_idWord;
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

    // QString cssSelector() const { return QStringLiteral("#%1 #%2 #%3").arg(paragraphId(), sentenceId(), wordId()); }


private:
    int m_idChapter;
    int m_idSection;
    int m_idParagraph;
    int m_idSentence;
    int m_idWord;
};

class Chapter {
public:
    Chapter(int id, const QString& name) :
        m_id(id), m_name(name) {}

    int id() const { return m_id; }
    const QString& name() const { return m_name; } 

    bool isInitialized() const { return sections.size() > 0; }

    QList<Section> sections;
    QList<Scene> scenes;

private:
    int m_id;
    QString m_name;
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
        m_id(id), m_text(text) {}

    bool operator< (const Word& o) const {
        return m_id < o.m_id;
    }

    int id() const { return m_id; }
    const QString& text() const { return m_text; }
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

