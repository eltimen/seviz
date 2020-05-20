#include "SentenceTree.h"
#include "frameelement.h" // TEST=====

SentenceTree::SentenceTree(IEngine* engine) :
    AbstractModule(engine, "SentenceTree"),
    m_feature("Деревья предложений", QIcon(":/st/icon.png"), new QDockWidget(), this, true),
    m_widget(this)
{
    m_feature.window()->setWidget(&m_widget);
    
    m_engine->registerHandler(EventType::MOUSE_OVER, ElementType::SENTENCE, CTRL, m_feature, SLOTPOS(onSentenceChanged));
    m_engine->registerHotkey(QKeySequence("Ctrl+F"), m_feature, [this]() {
        m_widget.onFrameInsert(m_engine);
    });
}

SentenceTree::~SentenceTree() {
    // TODO
}

QList<Feature> SentenceTree::features() {
    return { 
        m_feature
    };
}

void SentenceTree::render(const Position& from, const Position& to, DomChapter& dom, const QVector<Feature*>& activeFeatures) {
    if (m_currentSentenceData) {
        dom.addStyle(m_currentSentencePos.firstWord(), "border-left: 1px solid black;");
        Position pos = m_currentSentencePos.firstWord();
        while (pos.hasNextWord()) {
            pos = pos.nextWord();
        }
        dom.addStyle(pos, "border-right: 1px solid black;");

        dom.addStyle(m_currentSentencePos, "font-weight: bold;");
    }
}

SentenceData& SentenceTree::currentSentenceData() {
    return *m_currentSentenceData;
}

const FrameNetModel& SentenceTree::framesModel() {
    return m_framesModel;
}

void SentenceTree::onSentenceChanged(const Position& pos) {
    // TODO проверка на наличие несохраненных изменений

    m_currentSentencePos = pos.trimmedTo(ElementType::SENTENCE);
    m_currentSentence = m_engine->getBook().getSentence(m_currentSentencePos);
    if (m_storage.count(m_currentSentencePos)) {
        m_currentSentenceData = m_storage.at(m_currentSentencePos).get();
    } else {
        m_currentSentenceData = new SentenceData(m_currentSentence);
        m_storage.emplace(m_currentSentencePos, std::unique_ptr<SentenceData>(m_currentSentenceData));
    }

    m_widget.showSentence(m_currentSentence, *m_currentSentenceData);
    m_engine->triggerRerendering(m_currentSentencePos, m_currentSentencePos);
}



