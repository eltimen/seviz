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
	if (pos.paragraphId() != -1) { // при добавлении стиля к #viewer он почему-то исчезает. в firefox рабоотает нормально
		m_styles[pos] += css;
	}	
}

void DomChapter::addStyleToSpan(const Position& from, const Position& to, const QString& css) {
	if (!(from.hasSameLevelWith(to))) {
		throw std::invalid_argument("from must has same hierarchy level with to: sentence and sentence, ...");
	}
	if (!(from < to)) {
		throw std::invalid_argument("from must be < to");
	}

	Position current(from);
	current.setBook(m_chapter.book());
	do {
		addStyle(current, css);
		current = current.nextWord();
	} while (current.hasNextWord() && current.nextWord() <= to);
	for (int i = 0; i < (to.sentenceId() - from.sentenceId() - 1); ++i) {
		current = current.nextSentence();
		addStyle(current, css);
	}
	current = current.nextSentence().firstWord();
	while (current <= to) {
		addStyle(current, css);
		current = current.nextWord();
	} 


	// int deltaCh = to.chapterId() - from.chapterId();

	//int deltaSect = to.sectionId() - from.sectionId(); // not used yet
	//int deltaPar = to.paragraphId() - from.paragraphId();

	// расмотреть отдельно!!
	// int deltaSent = to.sentenceId() - from.sentenceId();
	// int deltaWord = to.wordId() - from.wordId();



}
