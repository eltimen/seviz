#include "EngineTest.h"

#include <QMessageBox>
#include <QMenu>

EngineTest::EngineTest(IEngine* engine)
    : AbstractModule(engine, "EngineTest"),
      m_widget(*this),
      m_feat("Тесты", QIcon(), new QDockWidget(), this, true, new QMenu("Тест")),
      m_feat2("Тесты 2", QIcon(), new QDockWidget(), this, true, new QMenu("Тест 2"))
{
    m_widget.m_engine = m_engine;
    m_feat.window()->setWidget(&m_widget);
    m_feat.menu()->addAction("Тест", [this]() { QMessageBox::information(m_feat.window(), "Сообщение", "Тест"); });
    m_feat.setDockLocation(Qt::RightDockWidgetArea);

    m_engine->registerHotkey(QKeySequence("Shift+A"), m_feat, SLOTVOID(EngineTest::handler));

    m_engine->registerHandler(EventType::MOUSE_OVER, ElementType::WORD, ALT, m_feat, [this](const Position& pos) {
        Word w = m_engine->getBook().getWord(pos);
        QMessageBox::information(m_feat.window(), "Обработчик", w.text());
    });

    // для тестирование болкировки/разблокировки конфликтущих feature
    m_engine->registerHandler(EventType::MOUSE_OVER, ElementType::WORD, ALT, m_feat2, [this](const Position& pos) {
        Word w = m_engine->getBook().getWord(pos);
        QMessageBox::information(m_feat2.window(), "Обработчик из второй feature", w.text());
    });
    m_engine->registerHotkey(QKeySequence("Shift+A"), m_feat2, [this]() {
        QMessageBox::information(m_feat2.window(), "Обработчик из второй feature", "Shift+A");
    });
}

EngineTest::~EngineTest() {
}

QList<Feature> EngineTest::features() {
    return { 
        m_feat,
        m_feat2
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
        if (renderStateChanged) {
            const Chapter& ch = m_engine->getBook().getCurrentChapter();
            dom.addStyleToSpan(Position(ch.id(), 1, 1, 1, 3), Position(ch.id(), 1, 2, 2, 1), "background-color: #ffe6e6;");
            renderStateChanged = false;
        }
        Position first = m_engine->getBook().getCurrentChapter().firstPos();
        dom.addStyle(first.nextParagraph().firstWord(), "color: green;");
        dom.addTailContent(first, BOTTOMRIGHT, "bottomright info");

        dom.addTooltip(first.firstSentence(), "Подсказка: это первое предложение");

        dom.addTailContent(Position(m_engine->getBook().getCurrentChapter().id(), 1, 1, 1, 3), BOTTOMRIGHT, "test");

        dom.addStyle(m_engine->getBook().getCurrentChapter().lastPos(), "color: blue; border: 1px solid black");
    } catch (std::out_of_range&) {
    } catch (std::range_error&) {}

}

void EngineTest::handler() {
    Position hover = m_engine->mouseHoverElement();
    QString hoverText = QStringLiteral("par %1 sent %2 word %3").arg(QString::number(hover.paragraphId()), QString::number(hover.sentenceId()), QString::number(hover.wordId()));
    QMessageBox::information(m_feat.window(), "Элемент под курсором", hoverText);
}


