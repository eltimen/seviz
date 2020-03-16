#include "EngineTest.h"
#include <QMessageBox>
#include "Book.h"

EngineTest::EngineTest(ModuleManager* engine) :
    AbstractModule(engine, "EngineTest"),
    m_widget(*this),
    m_feat("Тесты движка", QIcon(), new QDockWidget(), this, true)
{
    m_widget.m_engine = m_engine;
    m_feat.window()->setWidget(&m_widget);
    m_feat.setDockLocation(Qt::RightDockWidgetArea);

    m_engine->registerHotkey(QKeySequence("Shift+A"), m_feat, VOIDSLOT(EngineTest::handler));

    m_engine->registerHandler(EventType::MOUSE_OVER, ElementType::WORD, ALT, m_feat, [this](const Position& pos) {
        Word w = m_engine->getBook().getWord(pos);
        QMessageBox::information(m_feat.window(), "Обработчик", w.text());
    });
}

EngineTest::~EngineTest() {
}

QList<Feature> EngineTest::features() {
    return { 
        m_feat
    };
}

void EngineTest::load(QDir* dir) {
    if (dir) {
        QFile file(dir->filePath("data.txt"));
        if (file.open(QIODevice::ReadOnly)) {
            data = QString(file.readLine());
            m_widget.setText(data);
        }
        file.close();
    }   
}

void EngineTest::save(QDir& dir) {
    QFile file(dir.filePath("data.txt"));
    if (file.open(QIODevice::WriteOnly)) {
        file.write(data.toLocal8Bit());
    }
    file.close();
}

void EngineTest::render(const Position& from, const Position& to, DomChapter& dom, const QVector<Feature*>& activeFeatures) {
    try {
        const Chapter& ch = m_engine->getBook().getCurrentChapter();
        dom.addStyleToSpan(Position(ch.id(), 1, 1, 1, 3), Position(ch.id(), 1, 2, 2, 1), "background-color: #ffe6e6;");
        dom.addStyle(m_engine->getBook().getCurrentChapter().firstPos(), "color: green;");
        dom.addStyle(m_engine->getBook().getCurrentChapter().lastPos(), "color: blue; border: 1px solid black");
    } catch (std::out_of_range&) {}
}

void EngineTest::handler() {
    Position hover = m_engine->mouseHoverElement();
    QString hoverText = QStringLiteral("par %1 sent %2 word %3").arg(QString::number(hover.paragraphId()), QString::number(hover.sentenceId()), QString::number(hover.wordId()));
    QMessageBox::information(m_feat.window(), "Элемент под курсором", hoverText);
}


