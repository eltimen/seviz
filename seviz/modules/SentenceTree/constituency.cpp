#include "constituency.h"

#include <QString>
#include <QDebug>

using std::make_pair;

ConstituencyTree::ConstituencyTree(const Sentence& sent) {
    ChildrenContainer tokens;
    for (const Word& token : sent) {
        tokens.push_back(new ConstituencyTreeNode(token, ++m_lastId));
    }
    m_root = new ConstituencyTreeNode(S, ++m_lastId);
    m_root->setChildren(tokens);
}

ConstituencyTree::~ConstituencyTree() {
    delete m_root;
}

int ConstituencyTree::insert(const std::pair<int, int>& range, ConstituencyLabel label) {
    // TODO проверить на цикл (пересечение диапазонов или совпадение диапазонов)

    ConstituencyTreeNode* newNode = new ConstituencyTreeNode(label, ++m_lastId);
    // поиск ближайшего предка для добавляемой вершины и вставка в него
    ConstituencyTreeNode* parent = m_root->findParentFor(range);
    parent->replaceChildrenToNode(range, newNode);

    return m_lastId;
}

bool ConstituencyTree::canInsertNodeWithRange(int from, int to) const {
    return true;
}

bool ConstituencyTree::canChangeOrDeleteNode(int nodeId) const {
    return m_root->id() != nodeId && !m_root->find(nodeId)->isSentenceToken();
}

bool ConstituencyTree::change(int nodeId, ConstituencyLabel label) {
    if (canChangeOrDeleteNode(nodeId)) {
        ConstituencyTreeNode* node = m_root->find(nodeId);
        node->setLabel(label);
        return true;
    } else {
        return false;
    }
}

bool ConstituencyTree::remove(int nodeId) {
    if (canChangeOrDeleteNode(nodeId)) {
        m_root->removeNode(nodeId);
        return true;
    } else {
        return false;
    }
}

QString ConstituencyTree::toBracedString(const QString& sep) const {
    return m_root->toBracedString(sep);
}

QString ConstituencyTree::toTreantJson() const {
    QString ret = QStringLiteral(R"(
    {
    "chart": {
        "container": "#OrganiseChart-simple",
        "levelSeparation": 14,
        "siblingSeparation": 5,
        "subTeeSeparation": 10,
        "connectors": {
            "type": "step",
            "style": { "arrow-end": "open-wide-long" }
        }
    },
    "nodeStructure": 
    )");
    m_root->toTreantJson(ret, 0, m_root->maxDepth());
    ret += "}";
    return ret;
}

// ----------------------------------------------------------------------------------------

ConstituencyTreeNode::ConstituencyTreeNode(ConstituencyLabel label, int id)
    : m_id(id),
      m_isTerminal(false),
      m_label(label),
      m_token(1, "")
{
    m_range = std::make_pair(-1, -1);
}

ConstituencyTreeNode::ConstituencyTreeNode(const Word& token, int id)
    : m_id(id),
      m_isTerminal(true),
      m_label(),
      m_token(token) 
{
    m_range = std::make_pair(token.id(), token.id());
}

ConstituencyTreeNode::~ConstituencyTreeNode() {
    for (ConstituencyTreeNode* child : m_children) {
        delete child;
    }
}

int ConstituencyTreeNode::id() const {
    return m_id;
}

std::pair<int, int> ConstituencyTreeNode::tokenRange() const {
    return m_range;
}

bool ConstituencyTreeNode::isSentenceToken() const {
    return m_isTerminal;
}

void ConstituencyTreeNode::setChildren(ChildrenContainer children) {
    m_children = children;
    m_range = make_pair(children[0]->m_range.first, children[children.size()-1]->m_range.second);
}

void ConstituencyTreeNode::setLabel(ConstituencyLabel label) {
    assert(!m_isTerminal);
    m_label = label;
}

void ConstituencyTreeNode::removeNode(int nodeId) {
    for (ChildrenContainer::iterator it = m_children.begin(); it != m_children.end(); ++it) {
        ConstituencyTreeNode* child = *it;
        if (child->m_id == nodeId) {
            // когда удаляемый потомок найден, забираем у него его дочерние узлы
            ChildrenContainer children = child->m_children;
            // и вставляем их вместо данного узла
            it = m_children.erase(it);
            it = m_children.insert(it, children.begin(), children.end());
            break;
        } else {
            child->removeNode(nodeId);
        }
    }
}

void ConstituencyTreeNode::replaceChildrenToNode(const std::pair<int, int>& range, ConstituencyTreeNode* node) {
    assert(!m_isTerminal);
    ChildrenContainer newNodeChildren;

    decltype(m_children)::iterator it = std::find_if(m_children.begin(), m_children.end(), [&range](const auto& child) { 
        return child->isInsideOfRange(range) && child->tokenRange().first == range.first; 
    });

    newNodeChildren.push_back(*it);
    it = m_children.erase(it);
    it = m_children.insert(it, node);
    it++;
    
    while (it != m_children.end() && (*it)->isInsideOfRange(range)) {
        newNodeChildren.push_back(*it);
        it = m_children.erase(it);
    }

    node->setChildren(newNodeChildren);
}

ConstituencyTreeNode* ConstituencyTreeNode::findParentFor(const std::pair<int, int>& range) {
    ConstituencyTreeNode* found = this;
    for (auto& child : m_children) {
        if (child->isIncludesRange(range)) {
            found = child->findParentFor(range);
            break;
        }
    }
    assert(!found->m_isTerminal);
    return found;
}

ConstituencyTreeNode* ConstituencyTreeNode::find(int nodeId) {
    if (this->m_id == nodeId) {
        return this;
    }
    for (const auto& child : m_children) {
        ConstituencyTreeNode* found = nullptr;
        if (found = child->find(nodeId)) {
            return found;
        }
    }
    return nullptr;
}

int ConstituencyTreeNode::maxDepth() const {
    int depth = 0;
    for (const auto& child : m_children) {
        int d = child->maxDepth();
        depth = std::max(depth, d);
    }
    return depth + 1;
}

QString ConstituencyTreeNode::toBracedString(const QString& sep) const {
    assert(sep.size() == 2);

    if (m_isTerminal) {
        return m_token.text();
    } else {
        QString ret = sep[0];
        ret.append(ConstituencyLabelStr[m_label] + " ");

        for (const auto& child : m_children) {
            ret.append(child->toBracedString(sep));
            ret.append(" ");
        }
        ret.append(sep[1]);
        return ret;
    }
}

void ConstituencyTreeNode::toTreantJson(QString& ret, int depth, int maxDepth) const {
    
    int dropLevel = m_isTerminal ? maxDepth - 1 - depth : 0;
    for (int i = 0; i < dropLevel; ++i) {
        ret.append("{ pseudo: true, children: [");
    }

    ret += QStringLiteral(R"(
        {
        "text": { "name": "%1" },
        "HTMLclass": "%2",
        "HTMLid": "%3",
        "children": [
    )").arg(m_isTerminal ? m_token.text() : ConstituencyLabelStr[m_label])
        .arg(m_isTerminal ? "token" : (depth == 0 ? "label root" : "label"))
        .arg(QString::number(m_id));

    for (const ConstituencyTreeNode* child : m_children) {
        child->toTreantJson(ret, depth+1, maxDepth);
        ret += ",";
    }

    ret += "]} \n";

    for (int i = 0; i < dropLevel; ++i) {
        ret.append("]}");
    }
}

bool ConstituencyTreeNode::isIncludesRange(const std::pair<int, int>& range) const {
    const std::pair<int, int>& a = m_range;
    const std::pair<int, int>& b = range;

    assert(a.first <= a.second);
    assert(b.first <= b.second);

    //b внутри a
    return b.first >= a.first && b.second <= a.second;
}

bool ConstituencyTreeNode::isInsideOfRange(const std::pair<int, int>& range) const {
    const std::pair<int, int>& a = m_range;
    const std::pair<int, int>& b = range;

    assert(a.first <= a.second);
    assert(b.first <= b.second);

    //a внутри b
    return a.first >= b.first && a.second <= b.second;
}
