#pragma once
#include <exception>

class DuplicateModulesException : std::exception {
    const char* what() const override {
        return "Найдены модули с одинаковыми ID";
    }
};