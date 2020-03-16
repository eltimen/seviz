#include "DomChapter.h"
#include "Book.h"

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

	// начальные слова
	do {
		addStyle(current, css);
		current = current.nextWord();
	} while (current.hasNextWord() && current.nextWord() <= to);
	// обработка предложений и абзацев посередине
	// если from и to из одинаковых абзацев
	if (from.paragraphId() == to.paragraphId()) {
		for (int i = 0; i < (to.sentenceId() - from.sentenceId() - 1); ++i) {
			current = current.nextSentence();
			addStyle(current, css);
		}
		current = current.nextSentence().firstWord();
	} else {
		// если из разных абзацев, то
		// дообрабатываем предложения в абзаце из которого from
		int idLastSentence = m_chapter.book()->getParagraph(from).size();
		for (int i = 0; i < (idLastSentence - from.sentenceId()); ++i) {
			current = current.nextSentence();
			addStyle(current, css);
		}
		// обрабатываем абзацы посередине
		for (int i = 0; i < (to.paragraphId() - from.paragraphId() - 1); ++i) {
			current = current.nextParagraph();
			addStyle(current, css);
		}
		current = current.nextParagraph().firstSentence();
		// и начальные предложения абзаца to
		for (int i = 0; i < to.sentenceId() - 1; ++i) {
			addStyle(current, css);
			current = current.nextSentence();
		}
		current = current.firstWord();
	}
	// конечные слова
	while (current <= to) {
		addStyle(current, css);
		current = current.nextWord();
	} 
}
