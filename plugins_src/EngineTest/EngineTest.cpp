#include "EngineTest.h"

#include <QMessageBox>
#include <QMenu>

EngineTest::EngineTest()
    : ISevizPlugin(),
      m_feat("Тесты", QIcon(), new QDockWidget(), this, true, new QMenu("Тест")),
      m_widget(*this)
{
}

void EngineTest::init(IEngine* engine) {
    m_engine = engine;

    m_widget.m_engine = m_engine;
    m_feat.window()->setWidget(&m_widget);
    m_feat.menu()->addAction("Тест", [this]() { QMessageBox::information(m_feat.window(), "Сообщение", "Тест"); });
    m_feat.setDockLocation(Qt::RightDockWidgetArea);

    m_engine->registerHotkey(QKeySequence("Shift+A"), m_feat, SLOTVOID(EngineTest::handler));

    m_engine->registerHandler(EventType::MOUSE_OVER, ElementType::WORD, ALT, m_feat, [this](const Position& pos) {
        Word w = m_engine->getBook().getWord(pos);
        QMessageBox::information(m_feat.window(), "Обработчик", w.text());
    });
}

EngineTest::~EngineTest() {
}

const QString &EngineTest::id() const {
    static QString id = "EngineTest";
    return id;
}

int EngineTest::version() const {
    return 0;
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

void EngineTest::render(const Position&, const Position&, DomChapter& dom, const QVector<Feature*>&) {
    try {
        if (renderStateChanged) {
            const Chapter& ch = m_engine->getBook().getCurrentChapter();
            dom.addStyleToSpan(Position(ch.id(), 1, 1, 1, 3), Position(ch.id(), 1, 2, 2, 1), "background-color: #ffe6e6;");
            renderStateChanged = false;
        }
        Position first = m_engine->getBook().getCurrentChapter().firstPos();
        dom.addStyle(first, "color: green;");
        dom.addTailContent(first, TOPLEFT, "topleft");
        dom.addTailContent(first, TOPRIGHT, "topright");
        dom.addTailContent(first, BOTTOMLEFT, "bottomleft");
        dom.addTailContent(first, BOTTOMRIGHT, "bottomright");

        dom.addTooltip(first, BOTTOMRIGHT, "bottom right tail tooltip");
        dom.addTooltip(first.firstSentence(), "first sentence");

        dom.addTooltip(Position(m_engine->getBook().getCurrentChapter().id(), 1, 1, 1, 3), "Третье слово");
        dom.addTailContent(Position(m_engine->getBook().getCurrentChapter().id(), 1, 1, 1, 3), BOTTOMRIGHT, "test");

        dom.addStyle(m_engine->getBook().getCurrentChapter().lastPos(), "color: blue; border: 1px solid black");
    } catch (std::out_of_range&) {}
}

void EngineTest::handler() {
    Position hover = m_engine->mouseHoverElement();
    QString hoverText = QStringLiteral("par %1 sent %2 word %3").arg(QString::number(hover.paragraphId()), QString::number(hover.sentenceId()), QString::number(hover.wordId()));
    QMessageBox::information(m_feat.window(), "Элемент под курсором", hoverText);
}


