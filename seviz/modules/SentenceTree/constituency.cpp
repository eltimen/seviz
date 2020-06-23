#include "constituency.h"

#include <algorithm>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
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

int ConstituencyTree::insert(const std::pair<int, int>& range, ConstituencyLabel label, NodeInsertPosition* position) {
    try {
        ConstituencyTreeNode* newNode = new ConstituencyTreeNode(label, ++m_lastId);
        if (position) {
            position->parent->replaceChildrenToNode(position->childrenRange, range, newNode);
        } else {
            NodeInsertPosition pos = m_root->findPositionToInsertNode(range);
            pos.parent->replaceChildrenToNode(pos.childrenRange, range, newNode);
        }
        return m_lastId;
    } catch (int) {
        return -1;
    }
}

bool ConstituencyTree::canInsertNodeWithRange(int from, int to, NodeInsertPosition* position) const {
    try {
        NodeInsertPosition pos = m_root->findPositionToInsertNode(std::make_pair(from, to));
        if (position) {
            *position = pos;
        }
        return true;
    } catch (int) {
        return false;
    }
}

bool ConstituencyTree::canChangeOrDeleteNode(int nodeId) const {
    return m_root->id() != nodeId && !m_root->find(nodeId)->isSentenceToken();
}

void ConstituencyTree::fromJson(const QString& json) {

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8(), &error);
    if (!doc.isNull()) {
        QString labelStr = doc.object().value("label").toString();
        ConstituencyLabel label = static_cast<ConstituencyLabel>(ConstituencyLabelStr.indexOf(labelStr));
        int from = doc.object().value("fromWordId").toInt();
        int to = doc.object().value("toWordId").toInt();
        QJsonArray children = doc.object().value("children").toArray();
        
        fromJsonHelper(children);
    } else {
        throw QString("constituency ") + error.errorString();
    }
}

void ConstituencyTree::fromJsonHelper(const QJsonArray& children) {
    for (const QJsonValue& val : children) {
        QString labelStr = val.toObject().value("label").toString();
        ConstituencyLabel label = static_cast<ConstituencyLabel>(ConstituencyLabelStr.indexOf(labelStr));
        int from = val.toObject().value("fromWordId").toInt();
        int to = val.toObject().value("toWordId").toInt();
        QJsonArray children = val.toObject().value("children").toArray();

        fromJsonHelper(children);

        insert(std::make_pair(from, to), label);
    }
}

QString ConstituencyTree::toJson() {
    return m_root->toJson();
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

void ConstituencyTree::fromBracedString(const QString& str, const QString& sep) {
    QString bracedTree = str.simplified();
    bracedTree.replace(sep[0], sep[0] + QString(" "));
    bracedTree.replace(sep[1], QString(" ") + sep[1]);
    QStringList tokens = bracedTree.split(" ", QString::SplitBehavior::SkipEmptyParts);
    assert(tokens.front() == sep[0]);
    assert(tokens.back() == sep[1]);

    delete m_root;
    int i = 1;
    int i2 = 0;
    m_root = ConstituencyTreeNode::createNodeFromBracedString(tokens, i, i2, sep);
}

ConstituencyTreeNode* ConstituencyTreeNode::createNodeFromBracedString(const QStringList& tokens, int& indexTokenFrom, int& lastTokenId, const QString& sep) {
    QString label;
    ChildrenContainer children;
    QString text;
    for (int& i = indexTokenFrom; i < tokens.size(); ++i) {
        QString token = tokens[i];
        if (token == sep[0]) { // (
            if (i + 1 < tokens.size() && tokens[i + 1] == sep[1]) {
                ++lastTokenId;
                ++i;
                ++i;
                return new ConstituencyTreeNode(Word(lastTokenId, sep[0]), lastTokenId);
            } else {
                // добавить как потомка к текущей
                children.push_back(createNodeFromBracedString(tokens, ++i, lastTokenId, sep));
            }
        } else if (token == sep[1]) { // )
            // вернуть текущий узел
            if (!text.isEmpty()) {
                ++lastTokenId;
                return new ConstituencyTreeNode(Word(lastTokenId, text), lastTokenId);
            } else {
                int index = ConstituencyLabelStr.indexOf(label);
                ConstituencyTreeNode* ret;
                if (index == -1) {
                    if (label == "ROOT" && children.size() == 1) {
                        // для Stanford CoreNLP
                        return children[0];
                    } else {
                        ret = new ConstituencyTreeNode(NP, i);
                    }
                } else {
                    ret = new ConstituencyTreeNode(static_cast<ConstituencyLabel>(index), i);
                }
                ret->setChildren(children);
                return ret;
            }
        } else { // POS tag, constituency tag, or word?
            if (label.isEmpty()) {
                label = token;
                if (i+1 < tokens.size() && tokens[i+1] != sep[0]) {
                    ++i;
                    text = tokens[i];
                }
            } else {
                assert(false);
                return nullptr;
            }
        }
    }
    assert(false);
    return nullptr;
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
            child->m_children.clear();
            delete child;
            // и вставляем их вместо данного узла
            it = m_children.erase(it);
            it = m_children.insert(it, children.begin(), children.end());
            break;
        } else {
            child->removeNode(nodeId);
        }
    }
}

NodeInsertPosition ConstituencyTreeNode::findPositionToInsertNode(const std::pair<int, int>& range) {
    ConstituencyTreeNode* parent = findParentFor(range);
    if (parent) {
        decltype(m_children)::iterator it = std::find_if(parent->m_children.begin(), parent->m_children.end(), [&range](const auto& child) {
            return child->tokenRange().first == range.first && child->isInsideOfRange(range);
        });
        if (it == parent->m_children.end()) {
            throw -2;
        }
        ChildrenContainer::iterator from = it;

        while (it != parent->m_children.end() && (*it)->isInsideOfRange(range)) {
            it++;
            if (it != parent->m_children.end() && range.second >= (*it)->m_range.first && range.second < (*it)->m_range.second) {
                throw -3;
            }
        }

        return NodeInsertPosition(parent, from, it);
    }
    throw -1;
}

void ConstituencyTreeNode::replaceChildrenToNode(const ItersPair& iters, const std::pair<int, int>& range, ConstituencyTreeNode* node) {
    assert(!m_isTerminal);
    ChildrenContainer newNodeChildren(iters.first, iters.second);
    ChildrenContainer::iterator it = m_children.erase(iters.first, iters.second);
    m_children.insert(it, node);
    node->setChildren(newNodeChildren);
}

ConstituencyTreeNode* ConstituencyTreeNode::findParentFor(const std::pair<int, int>& range) const {
    const ConstituencyTreeNode* found = this;
    for (auto& child : m_children) {
        if (child->isIncludesRange(range) && !child->m_isTerminal) {
            found = child->findParentFor(range);
            break;
        }
    }
    assert(!found->m_isTerminal);
    return (ConstituencyTreeNode*)found;
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

QString ConstituencyTreeNode::toJson() {
    QString ret = QStringLiteral(R"(
		{
          "label": "%1",
          "fromWordId": %2,
          "toWordId": %3,
          "children": [
    )").arg(ConstituencyLabelStr[m_label])
        .arg(QString::number(m_range.first))
        .arg(QString::number(m_range.second));

    int i = 0;
    for (const auto& child : m_children) {
        if (!child->m_isTerminal) {
            if (i) ret.append(",");
            ret += child->toJson();
            ++i;
        }
    }

    ret += "] }";
    return ret;
}

QString ConstituencyTreeNode::toBracedString(const QString& sep) const {
    assert(sep.size() == 2);

    if (m_isTerminal) {
        if (m_token.text() == sep[0]) {
            return "( " + m_token.POS() + " " + sep[0] + sep[1]+" )";
        }
        return "(" + m_token.POS() + " " + m_token.text() + ")";
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
