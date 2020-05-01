#include "frameelement.h"
#include "framenet.h"

bool FrameElement::isWord() const {
    return !m_subFrame;
}

bool FrameElement::isFrame() const {
    return m_subFrame;
}

QString FrameElement::name() const {
    return m_name;
}

WordRange FrameElement::range() const {
    if (m_subFrame) {
        return m_subFrame->range();
    } else {
        return WordRange(m_words.begin()->id(), (m_words.end() - 1)->id());
    }
}

Frame* FrameElement::childFrame() const {
    return m_subFrame;
}

QList<Word> FrameElement::words() const {
    return m_words;
}

void FrameElement::toTreantJson(QString& ret, int depth, int maxDepth) const {
    if (isFrame()) {
        m_subFrame->toTreantJson(ret, depth, maxDepth, m_name);
    } else {
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
