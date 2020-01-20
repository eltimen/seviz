#pragma once
#include <list>

/*
    Для добавления модуля нужно в этом файле:
    1. Заинклудить header класса модуля
    2. В функции ниже добавить создание объекта класса модуля (обязательно через new!)
*/

#include "SyntaxTree/SyntaxTree.h"

class AbstractModule;
std::list<AbstractModule*> getSevizModulePointers() {
    return
    {
        new SyntaxTree()
    };
}