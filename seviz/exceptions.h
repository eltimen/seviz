#pragma once
#include <exception>
#include <stdexcept>
#include <string>
#include <QString>


class SevizException  {
public:
    SevizException() {}

    SevizException(const std::string& description) :
        m_descr(QString::fromStdString(description)) {}
    
    SevizException(const QString& description) :
        m_descr(description) {}
  
    SevizException(const char* description) :
        m_descr(description) {}
    

    QString description() const { return m_descr; }

    virtual QString what() const { return m_descr; }

private:
    QString m_descr;
};

class DuplicateModulesException : public SevizException {
    QString what() const override {
        return "Найдены модули с одинаковыми ID";
    }
};

class EmptySelectionException {};
