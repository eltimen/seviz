#include "SentenceTree.h"
#include "ModuleManager.h"
#include "Book.h"

SentenceTree::SentenceTree(ModuleManager* engine) : 
    AbstractModule(engine, "SentenceTree"),
    m_feature("Деревья предложений", QIcon(":/st/icon.png"), new QDockWidget(), this)
{
    m_feature.window()->setWidget(&m_widget);
    
    m_engine->registerHandler(EventType::MOUSE_OVER, ElementType::SENTENCE, CTRL, m_feature, POSSLOT(onSentenceChanged));
}

SentenceTree::~SentenceTree() {
}

QList<Feature> SentenceTree::features() {
    return { 
        m_feature
    };
}

void SentenceTree::onSentenceChanged(const Position& pos) {
    // TODO проверка на наличие несохраненных изменений
    // TODO если для такой pos предложение было загружено, показать его

    // иначе
    m_currentSentence = m_engine->getBook().getSentence(pos);
    SentenceData trees(m_currentSentence);
    m_storage.insert(pos, trees);
    m_widget.showSentence(m_currentSentence, trees);
}



