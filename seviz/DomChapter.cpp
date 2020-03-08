#include "DomChapter.h"

DomChapter::DomChapter(const Chapter& chapter) : 
	QObject(nullptr),
	m_chapter(chapter)
{
}

DomChapter::~DomChapter()
{
}
