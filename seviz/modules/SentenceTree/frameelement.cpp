#include "frameelement.h"

#include <memory>
#include "framenet.h"

FrameElement::FrameElement(const QString& name, Frame* child)
    : m_subFrame(child),
    m_name(name) 
{
    assert(child->range().first != -1);
}

FrameElement::FrameElement(const QString& name, std::shared_ptr<Frame> child) 
    : m_subFrame(child),
    m_name(name)
{
    assert(child->range().first != -1);
}

FrameElement::FrameElement(const QString& name, const std::vector<Word>& phrase)
    : m_words(phrase),
    m_name(name) {
    assert(phrase.size() > 0);
    // TODO assert слова в фразе должны быть соседями
}

FrameElement::~FrameElement() {
}

bool FrameElement::isWord() const {
    return !m_subFrame;
}

bool FrameElement::isFrame() const {
    return (bool)m_subFrame;
}

bool FrameElement::isEmpty() const {
    return m_name.isEmpty();
}

const QString& FrameElement::name() const {
    return m_name;
}

WordRange FrameElement::range() const {
    if (m_subFrame) {
        return m_subFrame->range();
    } else {
        return WordRange(m_words.begin()->id(), (m_words.end() - 1)->id());
    }
}

std::shared_ptr<Frame> FrameElement::childFrame() const {
    return m_subFrame;
}

const std::vector<Word>& FrameElement::words() const {
    return m_words;
}

void FrameElement::toTreantJson(QString& ret, int depth, int maxDepth) const {
    if (isFrame()) {
        m_subFrame->toTreantJson(ret, depth, maxDepth, m_name);
    } else if (!isEmpty()) { // TODO fix empty FEs bug
        int dropLevel = maxDepth - 1 - depth;

        for (int i = 0; i < dropLevel; ++i) {
            ret.append("{ pseudo: true, children: [");
        }

        QString constituent;
        for (const auto& w : m_words) {
            constituent += w.text() + " ";
        }

        ret += QStringLiteral(R"(
        {
        "text": { title: "%1", name: "%2" }, 
        "HTMLclass": "%3",
        "children": [
        )").arg(m_name)
            .arg(constituent)
            .arg("word");

        ret += "]} \n";

        for (int i = 0; i < dropLevel; ++i) {
            ret.append("]}");
        }
    }
    

   

    
}
