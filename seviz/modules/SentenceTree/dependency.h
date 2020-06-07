#pragma once

#include <map>
#include <algorithm>
#include <QString>
#include "BookModels.h"

// taken from "Stanford typed dependencies manual", section 2
#define DEPENDENCY_RELATION(DO) \
    DO(acomp) \
    DO(advcl) \
    DO(advmod) \
    DO(agent) \
    DO(amod) \
    DO(appos) \
    DO(aux) \
    DO(auxpass) \
    DO(cc) \
    DO(ccomp) \
    DO(conj) \
    DO(cop) \
    DO(csubj) \
    DO(csubjpass) \
    DO(dep) \
    DO(det) \
    DO(discourse) \
    DO(dobj) \
    DO(expl) \
    DO(goeswith) \
    DO(iobj) \
    DO(mark) \
    DO(mwe) \
    DO(neg) \
    DO(nn) \
    DO(npadvmod) \
    DO(nsubj) \
    DO(nsubjpass) \
    DO(num) \
    DO(number) \
    DO(parataxis) \
    DO(pcomp) \
    DO(pobj) \
    DO(poss) \
    DO(possessive) \
    DO(preconj) \
    DO(predet) \
    DO(prep) \
    DO(prepc) \
    DO(prt) \
    DO(punct) \
    DO(quantmod) \
    DO(rcmod) \
    DO(ref) \
    DO(root) \
    DO(tmod) \
    DO(vmod) \
    DO(xcomp) \
    DO(xsubj) 

#define MAKE_ENUM(VAR) VAR,
enum class DependencyRelation {
    DEPENDENCY_RELATION(MAKE_ENUM)
};

#define MAKE_STRINGS(VAR) QStringLiteral(#VAR),
const QStringList DependencyRelationStr = {
    DEPENDENCY_RELATION(MAKE_STRINGS)
};

#undef MAKE_STRINGS
#undef MAKE_ENUM

class DependencyTree {
public:
    DependencyTree(const Sentence& sent);
   
    // добавить ребро
    // in: ID откуда, ID куда, тип связи
    bool insert(int from, int to, DependencyRelation type);
    // удалить ребро
    // in: ID откуда, ID куда
    void remove(int from, int to);
    // изменить тип связи
    // in: ID откуда, ID куда, новый тип связи
    void change(int from, int to, DependencyRelation type);

    bool canInsertRelation(int from, int to) const;

    void fromStanfordCoreNlpJson(const QString& basicDepJson);
    QString toStanfordCoreNlpJson() const;

    QString toBratJson() const;

private:
    Sentence m_sentence;
    // adjacency list
    // word id -> [ {word id, stanford relation}, ...]
    std::multimap<int, std::pair<int, DependencyRelation>> m_tree;

    bool hasLoopWithEdge(int from, int to) const;
    bool hasLoopWithEdgeUtil(int v, std::vector<bool>& visited, std::vector<bool>& recStack, int from, int to) const;
    inline bool hasEdge(int from, int to) const; 
};