#pragma once

#include <map>
#include <QString>
#include "BookModels.h"

enum DependencyRelation {
    nsubj
};

// TODO функция добавления ребра
   // вход - ID откуда, ID куда, тип связи

   // TODO функция удаления ребра
   // вход - ID откуда, ID куда или ID ребра

   // TODO функция изменения типа ребра
   // вход - ID откуда, ID куда или ID ребра

class DependencyTree {
public:
    DependencyTree(const Sentence& sent);
   
    void insert(int from, int to, DependencyRelation type);
    void remove(int from, int to);
    void change(int from, int to, DependencyRelation type);

    QString toBratJson() const;

private:
    bool hasLoop() const;

    // adjacency list
    // word id -> <relation, word id>, <relation, word id>, ...
    std::multimap<int, std::pair<int, DependencyRelation>> m_tree;
};