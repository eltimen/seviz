include(../../includes.pri)

QT       += widgets

TARGET = EngineTest
TEMPLATE = lib

DEFINES += ENGINETEST_LIBRARY

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

Debug:SEVIZ_DESTDIR = ../../core/debug
Release:SEVIZ_DESTDIR = ../../core/release

# path to seviz header files
INCLUDEPATH += ../../core/
LIBS += $$SEVIZ_DESTDIR/seviz.lib

SOURCES += \
        enginetest.cpp \
        enginetestwidget.cpp

HEADERS += \
        enginetest.h \
        enginetestwidget.h
		
FORMS += \
        enginetestwidget.ui

# copy plugin dll to main executable's plugins/ folder
copyToDir($$OUT_PWD/$$DESTDIR/$${TARGET}.dll, $$SEVIZ_DESTDIR/plugins/)

unix {
    target.path = /usr/lib
    INSTALLS += target
}
