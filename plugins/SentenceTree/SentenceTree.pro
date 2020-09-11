include(../../includes.pri)

QT       += widgets webengine webenginewidgets webchannel sql

TARGET = SentenceTree
TEMPLATE = lib

DEFINES += SENTENCETREE_LIBRARY

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

Debug:SEVIZ_DESTDIR = ../../core/debug
Release:SEVIZ_DESTDIR = ../../core/release

# path to seviz header files
INCLUDEPATH += ../../core/
LIBS += $$SEVIZ_DESTDIR/seviz.lib

SOURCES += \
        choosepalettedialog.cpp \
        constituency.cpp \
        dependency.cpp \
        frame.cpp \
        frameelement.cpp \
        frameelementseditor.cpp \
        framenet.cpp \
        framenetmodel.cpp \
        SentenceTree.cpp \
        stwindow.cpp \
        wordrange.cpp

HEADERS += \
        choosepalettedialog.h\
        constituency.h \
        dependency.h \
        frame.h \
        frameelement.h \
        frameelementseditor.h \
        framenet.h \
        framenetmodel.h \
        SentenceTree.h \
        stwindow.h \
        wordrange.h
		
FORMS += \
        choosepalettedialog.ui \
        frameelementseditor.ui \
        stwindow.ui
		 
RESOURCES += sentencetree.qrc
		
# copy assets to plugin folder
CONFIG += file_copies
webcore.files = SentenceTree_resources/
webcore.path = $$DESTDIR/
COPIES += webcore
# copy plugin assets and dll to main executable's plugins/ folder
copyToDir($$OUT_PWD/$$DESTDIR/$${TARGET}.dll, $$SEVIZ_DESTDIR/plugins/)
webcore.path = $$SEVIZ_DESTDIR/plugins/
COPIES += webcore

unix {
    target.path = /usr/lib
    INSTALLS += target
}
