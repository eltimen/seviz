#pragma once

#include <QObject>
#include <QMap>
#include "BookModels.h"
#include "exceptions.h"

class SEVIZSHARED_EXPORT DomChapter : public QObject
{
	Q_OBJECT

public:
	DomChapter(const Chapter& chapter);
	~DomChapter();

	int chapterId() const { return m_chapter.id(); }

	void addStyle(const Position& pos, const QString& css);
	void addStyleToSpan(const Position& from, const Position& to, const QString& css);
	void addTailContent(const Position& tailPos, const QString& tailText);
	void addTailContent(const Position& pos, TailPosition where, const QString& tailText);
	void addTooltip(const Position& pos, const QString& text);
	void addTooltip(const Position& pos, TailPosition where, const QString& tooltipText);

	const QMap<Position, QString>& getStyles() const { return m_styles; }
	const QMap<Position, QString>& getTails() const { return m_tails; }
	const QMap<Position, QString>& getTooltips() const { return m_tooltips; }

private:
	QMap <Position, QString> m_styles;
	QMap <Position, QString> m_tails;
	QMap <Position, QString> m_tooltips;
	const Chapter& m_chapter;

	void addStyleToWordCount(Position& first, int n, const QString& css);
	void addStyleToSentenceCount(Position& first, int n, const QString& css);
	void addStyleToParagraphCount(Position& first, int n, const QString& css);
	void addStyleToSentenceSpan(Position& iter, const Position& from, const Position& to, const QString& css);
};
