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

	// TODO refactor

	assert(from.sectionId() != -1);
	assert(from.chapterId() == to.chapterId());

	if (from.wordId() != -1) {
		// начальные слова
		addStyle(from, css);
		while (current.hasNextWord() && current < to) {
			current = current.nextWord();
			addStyle(current, css);
		}
		if (from.sentenceId() != to.sentenceId()) {
			// обработка предложений и абзацев посередине
		// если from и to из одинаковых абзацев
			if (from.sentenceId() != to.sentenceId() && from.paragraphId() == to.paragraphId()) {
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
		}
		// конечные слова
		while (current <= to) {
			addStyle(current, css);
			current = current.nextWord();
		}
	} else if (from.sentenceId() != -1) {
		addStyle(from, css);
		while (current.hasNextSentence() && current < to) {
			current = current.nextSentence();
			addStyle(current, css);
		}
		for (int i = 0; i < (to.paragraphId() - from.paragraphId() - 1); ++i) {
			current = current.nextParagraph();
			addStyle(current, css);
		}
		current = current.nextParagraph().firstSentence();
		while (current < to) {
			addStyle(current, css);
			current = current.nextSentence();
		}
		addStyle(to, css);
	} else if (from.paragraphId() != -1) {
		addStyle(from, css);
		while (current.hasNextParagraph() && current < to) {
			current = current.nextParagraph();
			addStyle(current, css);
		}
		addStyle(to, css);
	}
	
}
