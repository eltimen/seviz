#ifndef CONSTITUENCY_H
#define CONSTITUENCY_H

#include <utility>
#include <memory>
#include <vector>
#include <QJsonArray>
#include "BookModels.h"

// taken from penn treebank
#define CONSTITUENCY_LABEL(DO) \
    DO(ADJP)\
    DO(ADVP)\
    DO(NP)\
    DO(PP)\
    DO(S)\
    DO(SBAR)\
    DO(SBARQ)\
    DO(SINV)\
    DO(SQ)\
    DO(VP)\
    DO(WHADVP)\
    DO(WHNP)\
    DO(WHPP)

#define MAKE_ENUM(VAR) VAR,
#define MAKE_STRINGS(VAR) QStringLiteral(#VAR),
enum ConstituencyLabel {
    CONSTITUENCY_LABEL(MAKE_ENUM)
};

const QStringList ConstituencyLabelStr = {
    CONSTITUENCY_LABEL(MAKE_STRINGS)
};

class ConstituencyTreeNode;

struct NodeInsertPosition {
    using ChildrenContainer = std::vector<ConstituencyTreeNode*>;
    NodeInsertPosition() = default;
    NodeInsertPosition(ConstituencyTreeNode* parent, ChildrenContainer::iterator& from, ChildrenContainer::iterator& to) 
        : parent(parent),
          childrenRange(std::make_pair(from, to))
    {
        assert(from <= to);
    }

    ConstituencyTreeNode* parent;
    std::pair<ChildrenContainer::iterator, ChildrenContainer::iterator> childrenRange;
};

class ConstituencyTreeNode {
public:
    using ChildrenContainer = std::vector<ConstituencyTreeNode*>;
    using ItersPair = std::pair<ChildrenContainer::iterator, ChildrenContainer::iterator>;

    ConstituencyTreeNode(ConstituencyLabel label, int id);
    ConstituencyTreeNode(const Word& token, int id);
    ~ConstituencyTreeNode();

    int id() const;
    std::pair<int, int> tokenRange() const;
    bool isSentenceToken() const;

    void setChildren(ChildrenContainer children);
    void setLabel(ConstituencyLabel label);
    void removeNode(int nodeId);
    void replaceChildrenToNode(const ItersPair&, const std::pair<int, int>& range, ConstituencyTreeNode* node);
    NodeInsertPosition findPositionToInsertNode(const std::pair<int, int>& range);
    ConstituencyTreeNode* find(int nodeId);
    int maxDepth() const;

    QString toJson();

    QString toBracedString(const QString& sep) const;
    void toTreantJson(QString& ret, int depth, int maxDepth) const;

private:
    int m_id;

    std::pair<int, int> m_range;

    bool m_isTerminal;
    ConstituencyLabel m_label;
    Word m_token;

    ChildrenContainer m_children;

    ConstituencyTreeNode* findParentFor(const std::pair<int, int>& range) const;
    bool isIncludesRange(const std::pair<int, int>& range) const;
    bool isInsideOfRange(const std::pair<int, int>& range) const;
};

class ConstituencyTree
{
    using ChildrenContainer = ConstituencyTreeNode::ChildrenContainer;
public:
    explicit ConstituencyTree(const Sentence& sent);
    ~ConstituencyTree();

    int insert(const std::pair<int, int>& range, ConstituencyLabel label, NodeInsertPosition* position = nullptr);
    bool change(int nodeId, ConstituencyLabel label);
    bool remove(int nodeId);

    bool canInsertNodeWithRange(int from, int to, NodeInsertPosition* position = nullptr) const;
    bool canChangeOrDeleteNode(int nodeId) const;

    // пока не будет интегрирован CoreNLP
    void fromJson(const QString& json);
    QString toJson();

    //void fromBracedString(const QString& str, int lastTokenId = 0, const QString& sep = "()");
    QString toBracedString(const QString& sep = "()") const;

    QString toTreantJson() const;

private:
    int m_lastId = 0;
    ConstituencyTreeNode *m_root;

    int createNodeFromBracedString(const QStringList& str, int lastTokenId, const QString& sep);
    void fromJsonHelper(const QJsonArray& children);
};

#endif // CONSTITUENCY_H
