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
	int id;
	QString name;
};

class Paragraph : public QList<Sentence> {
	int id;
	QString text;
};

class Sentence : public QList<Word> {
	int id;
	int idWordStart;
	int idWordEnd;
};

class Word {
	int id;
	QString text;
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

