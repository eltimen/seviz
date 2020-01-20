#-------------------------------------------------
#
# Project created by QtCreator 2020-01-12T11:48:05
#
#-------------------------------------------------

QT       += core gui webengine webenginewidgets webchannel

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = seviz
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++14

SOURCES += \
        AbstractModule.cpp \
        Book.cpp \
        ModuleManager.cpp \
        epubrenderer.cpp \
        main.cpp \
        mainwindow.cpp \
        modules/SyntaxTree/SyntaxTree.cpp

HEADERS += \
        AbstractModule.h \
        Book.h \
        ModuleManager.h \
        epubrenderer.h \
        exceptions.h \
        mainwindow.h \
        modules/SyntaxTree/SyntaxTree.h \
        modules/modules.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
