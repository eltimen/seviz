#pragma once

#include <QObject>
#include "BookModels.h"

class DomChapter : public QObject
{
	Q_OBJECT

public:
	DomChapter(const Chapter& chapter);
	~DomChapter();

	int chapterId() const { return m_chapter.id(); }

	void addStyleTo(const Paragraph& elem, const QString& css);
	void addStyleTo(const Sentence& elem, const QString& css);
	void addStyleTo(const Word& elem, const QString& css);
	void addStyleToSpan(const Position& from, const Position& to);

	QString toHtml();

private:
	QVector <QPair<Position,Position>> m_styleIntervals;
	const Chapter& m_chapter;
};
