#include "SentenceTree.h"

SentenceTree::SentenceTree(IEngine* engine) :
    AbstractModule(engine, "SentenceTree"),
    m_feature("Деревья предложений", QIcon(":/st/icon.png"), new QDockWidget(), this, true),
    m_widget(this)
{
    m_feature.window()->setWidget(&m_widget);
    
    m_engine->registerHandler(EventType::MOUSE_OVER, ElementType::SENTENCE, CTRL, m_feature, SLOTPOS(onSentenceChanged));
}

SentenceTree::~SentenceTree() {
    // TODO
    delete m_currentSentenceData;
}

QList<Feature> SentenceTree::features() {
    return { 
        m_feature
    };
}

void SentenceTree::render(const Position& from, const Position& to, DomChapter& dom, const QVector<Feature*>& activeFeatures) {
    const Chapter& ch = m_engine->getBook().getCurrentChapter();
    for (const Paragraph& par : ch.sections.first()) {
        for (const Sentence& sent : par) {
            Position pos = Position(ch.id(), 1, par.id(), sent.id());
            //if (!m_storage.contains(pos) || m_storage.value(pos, SentenceData(sent)).dependencyState == NODATA) {
                dom.addStyle(pos, "background-color: #ffe6e6;");
            //}
        }
    }
}

SentenceData& SentenceTree::currentSentenceData() {
    return *m_currentSentenceData;
}

void SentenceTree::onSentenceChanged(const Position& pos) {
    // TODO проверка на наличие несохраненных изменений
    // TODO если для такой pos предложение было загружено, показать его

    // иначе
    m_currentSentence = m_engine->getBook().getSentence(pos);
    m_currentSentenceData = new SentenceData(m_currentSentence);
    //m_storage.insert(pos, *m_currentSentenceData);
    m_widget.showSentence(m_currentSentence, *m_currentSentenceData);

    //test constituency
    int newId = m_currentSentenceData->constituency.insert(std::make_pair(1, 3), ConstituencyLabel::NP);
    m_currentSentenceData->constituency.change(newId, ConstituencyLabel::SBAR);
    qDebug() << m_currentSentenceData->constituency.toBracedString();
}



