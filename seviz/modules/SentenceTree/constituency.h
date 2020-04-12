#ifndef CONSTITUENCY_H
#define CONSTITUENCY_H

#include <utility>
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
enum class ConstituencyLabel {
    CONSTITUENCY_LABEL(MAKE_ENUM)
};

#define MAKE_STRINGS(VAR) QStringLiteral(#VAR),
const QStringList ConstituencyLabelStr = {
    CONSTITUENCY_LABEL(MAKE_STRINGS)
};


class ConstituencyTreeNode {
    using ChildrenContainer=std::vector<ConstituencyTreeNode*>;

public:
    ConstituencyTreeNode(ConstituencyLabel label, int id);
    ConstituencyTreeNode(const Word& token, int id);
    ~ConstituencyTreeNode();

    std::pair<int, int> tokenRange() const;
    void setChildren(ChildrenContainer children);
    void replaceChildrenToNode(const std::pair<int, int>& range, ConstituencyTreeNode* node);
    ConstituencyTreeNode* findParentFor(const std::pair<int, int>& range);

private:
    int m_id;

    std::pair<int, int> m_range;

    bool m_isTerminal;
    ConstituencyLabel m_label;
    Word m_token;

    ChildrenContainer m_children;

    bool isIncludesRange(const std::pair<int, int>& range) const;
    bool isInsideOfRange(const std::pair<int, int>& range) const;
};

class ConstituencyTree
{
public:
    ConstituencyTree(const Sentence& sent);
    ~ConstituencyTree();

    int insert(const std::pair<int, int>& range, ConstituencyLabel label);
    void change(int nodeId, ConstituencyLabel label);
    void remove(int nodeId);

private:
    int m_lastId = 0;
    ConstituencyTreeNode *m_root;
};

#endif // CONSTITUENCY_H
