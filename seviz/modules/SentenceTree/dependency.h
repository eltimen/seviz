#pragma once

#include <map>
#include <QString>
#include "BookModels.h"

#define DEPENDENCY_RELATION(DO) \
    DO(nsubj) \
    DO(advd)

#define MAKE_ENUM(VAR) VAR,
enum DependencyRelation {
    DEPENDENCY_RELATION(MAKE_ENUM)
};

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

    QString toBratJson() const;

private:
    Sentence m_sentence;
    // adjacency list
    // word id -> [ {word id, stanford relation}, ...]
    std::multimap<int, std::pair<int, DependencyRelation>> m_tree;

    bool hasLoopWithEdge(int from, int to) const;
    bool hasLoopWithEdgeUtil(int v, std::vector<bool>& visited, std::vector<bool>& recStack, int from, int to) const;

    QString string(DependencyRelation rel) const;
};
