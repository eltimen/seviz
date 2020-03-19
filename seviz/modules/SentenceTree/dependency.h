#pragma once

#include <map>
#include <QString>
#include "BookModels.h"

enum DependencyRelation {
    nsubj
};

class DependencyTree {
public:
    DependencyTree(const Sentence& sent);
   
    // добавить ребро
    // in: ID откуда, ID куда, тип связи
    void insert(int from, int to, DependencyRelation type);
    // удалить ребро
    // in: ID откуда, ID куда
    void remove(int from, int to);
    // изменить тип связи
    // in: ID откуда, ID куда, новый тип связи
    void change(int from, int to, DependencyRelation type);

    QString toBratJson() const;

private:
    Sentence m_sentence;
    bool hasLoopWithRelation(int from, int to) const;

    // adjacency list
    // word id -> [ {word id, stanford relation}, ...]
    std::multimap<int, std::pair<int, DependencyRelation>> m_tree;
};