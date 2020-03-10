#include "DomChapter.h"

DomChapter::DomChapter(const Chapter& chapter) : 
	QObject(nullptr),
	m_chapter(chapter)
{
}

DomChapter::~DomChapter()
{
}

void DomChapter::addStyle(const Position& pos, const QString& css) {
	if (pos.paragraphId() != -1) { // при добавлении стиля к #viewer он почему-то исчезает
		m_styles[pos] += css;
	}	
}

