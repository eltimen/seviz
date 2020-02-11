#pragma once

#include <QList>
#include <QString>

struct Chapter;
class Section;
class Paragraph;
class Sentence;
class Word; // знаки пунктуации - пусть пока будут тоже словами

class Scene;
class Fragment;

class Position {
	int idChapter;
	int idSection;
	int idParagraph;
	int idSentence;
	int idWord;
};

struct Chapter {
	Chapter(int id, const QString& name) :
		id(id), name(name) {}

	int id;
	QString name;
	QList<Section> sections;
	QList<Scene> scenes;
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

