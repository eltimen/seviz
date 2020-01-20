#include "Book.h"
#include <exception>

Book::Book(const QString& epubPath, QWebEngineView* view) : 
    QObject(nullptr),
    m_renderer(view),
    m_epubPath(epubPath)
{
}

Book::~Book()
{
}

void Book::open() {
    // распаковать во временную папку
    // передать opf в epub.js
    m_renderer.open(m_epubPath); //TODO
    // если ошибка - бросить exception
}
