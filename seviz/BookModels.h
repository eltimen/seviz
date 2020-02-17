#pragma once

#include <QList>
#include <QString>

class Chapter;
class Section;
class Paragraph;
class Sentence;
class Word; // знаки пунктуации - пусть пока будут тоже словами

class Scene;
class Fragment;

class Position {
	//int idChapter;
	int idSection;
	int idParagraph;
	int idSentence;
	int idWord;
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

