#pragma once

#include <QObject>

class DomChapter : public QObject
{
	Q_OBJECT

public:
	DomChapter(QObject *parent);
	~DomChapter();
};
