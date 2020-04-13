#include "constituency.h"

using std::make_pair;

ConstituencyTree::ConstituencyTree(const Sentence& sent) {
    ChildrenContainer tokens;
    for (const Word& token : sent) {
        tokens.push_back(std::make_shared<ConstituencyTreeNode>(token, ++m_lastId));
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

void ConstituencyTree::change(int nodeId, ConstituencyLabel label) {
    ConstituencyTreeNode* node = m_root->find(nodeId);
    node->setLabel(label);
}

QString ConstituencyTree::toBracedString(const QString& sep) const {
    return m_root->toBracedString(sep);
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
}

std::pair<int, int> ConstituencyTreeNode::tokenRange() const {
    return m_range;
}

void ConstituencyTreeNode::setChildren(ChildrenContainer children) {
    m_children = children;
    m_range = make_pair(children[0]->m_range.first, children[children.size()-1]->m_range.second);
}

void ConstituencyTreeNode::setLabel(ConstituencyLabel label) {
    assert(!m_isTerminal);
    m_label = label;
}

void ConstituencyTreeNode::replaceChildrenToNode(const std::pair<int, int>& range, ConstituencyTreeNode* node) {
    assert(!m_isTerminal);
    ChildrenContainer newNodeChildren;

    decltype(m_children)::iterator it = std::find_if(m_children.begin(), m_children.end(), [&range](const auto& child) { 
        return child->isInsideOfRange(range); 
    });

    newNodeChildren.push_back(*it);
    it = m_children.erase(it);
    it = m_children.insert(it, std::unique_ptr<ConstituencyTreeNode>(node));
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
