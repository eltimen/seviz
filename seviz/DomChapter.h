#pragma once

#include <QObject>
#include <QMap>
#include "BookModels.h"

class DomChapter : public QObject
{
	Q_OBJECT

public:
	DomChapter(const Chapter& chapter);
	~DomChapter();

	int chapterId() const { return m_chapter.id(); }

	void addStyle(const Position& pos, const QString& css);
	void addStyleToSpan(const Position& from, const Position& to, const QString& css);

	const QMap<Position, QString>& getStyles() const { return m_styles; }

private:
	QMap <Position, QString> m_styles;
	const Chapter& m_chapter;
};
