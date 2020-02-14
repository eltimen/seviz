#pragma once
#include <exception>
#include <QString>

class qt_info_exception : std::exception {
public:
    qt_info_exception() {}

    qt_info_exception(const std::string& description) :
        m_descr(QString::fromStdString(description)) {}
    
    qt_info_exception(const QString& description) :
        m_descr(description) {}
  
    qt_info_exception(const char* description) :
        m_descr(description) {}
    

    QString getDescription() { return m_descr; }
private:
    QString m_descr;
};

class DuplicateModulesException : public qt_info_exception {
    const char* what() const override {
        return "Найдены модули с одинаковыми ID";
    }
};

class CantUnpackEpubException : qt_info_exception {
    using qt_info_exception::qt_info_exception;
};

class InvalidEpubException : qt_info_exception {
    using qt_info_exception::qt_info_exception;
};