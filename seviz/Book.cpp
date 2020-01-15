#include "Book.h"

Book::Book(QWebEngineView* view)
	: QObject(nullptr),
	m_renderer(view)
{
}

Book::~Book()
{
}
