#pragma once
#include <vector>

class AbstractModule;

/*
    Для добавления модуля нужно в этом файле:
    1. Заинклудить header класса модуля:    #include "MyModule/MyModule.h"
    2. Дописать в registrar: new MyModule(),
*/

// TODO сделать загрузку плагинов из dll в определенной папке, без необходимости в данном файле

#include "SentenceTree/SentenceTree.h"
#include "EngineTest/EngineTest.h"

std::vector<AbstractModule*> ModuleManager::registrar() {
    return
    {
        new SentenceTree(this),
        new EngineTest(this),
    };
};
