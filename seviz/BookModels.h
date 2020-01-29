#pragma once

#include <QList>
#include <QString>

struct Chapter;
class Section;
class Paragraph;
class Sentence;
class Word; // ����� ���������� - ����� ���� ����� ���� �������

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

class Section {
	int id;
	QString name;
	QList<Paragraph> paragraphs;
};

class Paragraph {
	int id;
	QString text;
	QList<Sentence> sentences;
};

class Sentence {
	int id;

	int idWordStart;
	int idWordEnd;
	QList<Word> words;
};

class Word {
	int id;
	QString text;
};

class Scene {
	int id;
	int idParagraphStart;
	int idParagraphEnd;
	QList<Fragment> fragments;
};

class Fragment {
	int id;
	Position positionStart;
	Position positionEnd;
};

