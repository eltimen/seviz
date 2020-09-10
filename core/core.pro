include(../includes.pri)

QT       += core gui webengine webenginewidgets webchannel sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

TARGET = seviz
TEMPLATE = app

CONFIG += c++17

DEFINES += QUAZIP_STATIC
INCLUDEPATH += 3rdparty/quazip/quazip
INCLUDEPATH += 3rdparty/zlib
# suppress some warnings from zlib
win32-msvc* {
    DEFINES += _CRT_SECURE_NO_WARNINGS
    DEFINES += _CRT_NONSTDC_NO_DEPRECATE
}

DEFINES += SEVIZ_CORE
SOURCES += \
        3rdparty/quazip/quazip/JlCompress.cpp \
        3rdparty/quazip/quazip/qioapi.cpp \
        3rdparty/quazip/quazip/quaadler32.cpp \
        3rdparty/quazip/quazip/quacrc32.cpp \
        3rdparty/quazip/quazip/quagzipfile.cpp \
        3rdparty/quazip/quazip/quaziodevice.cpp \
        3rdparty/quazip/quazip/quazip.cpp \
        3rdparty/quazip/quazip/quazipdir.cpp \
        3rdparty/quazip/quazip/quazipfile.cpp \
        3rdparty/quazip/quazip/quazipfileinfo.cpp \
        3rdparty/quazip/quazip/quazipnewinfo.cpp \
        3rdparty/quazip/quazip/quachecksum32.cpp \
        3rdparty/quazip/quazip/unzip.c \
        3rdparty/quazip/quazip/zip.c \
        3rdparty/zlib/adler32.c \
        3rdparty/zlib/compress.c \
        3rdparty/zlib/crc32.c \
        3rdparty/zlib/deflate.c \
        3rdparty/zlib/gzclose.c \
        3rdparty/zlib/gzlib.c \
        3rdparty/zlib/gzread.c \
        3rdparty/zlib/gzwrite.c \
        3rdparty/zlib/infback.c \
        3rdparty/zlib/inffast.c \
        3rdparty/zlib/inflate.c \
        3rdparty/zlib/inftrees.c \
        3rdparty/zlib/trees.c \
        3rdparty/zlib/uncompr.c \
        3rdparty/zlib/zutil.c \
        Book.cpp \
        BookModels.cpp \
        DomChapter.cpp \
        PluginManager.cpp \
        bookloader/bookloader.cpp \
        bookloader/bookloaderdialog.cpp \
        epubrenderer.cpp \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        3rdparty/quazip/quazip/quagzipfile.h \ #for moc
        3rdparty/quazip/quazip/quaziodevice.h \
        3rdparty/quazip/quazip/quazipfile.h \
        Book.h \
        BookModels.h \
        DomChapter.h \
        EventModels.h \
        IEngine.h \
        ISevizPlugin.h \
        PluginManager.h \
        bookloader/bookloader.h \
        bookloader/bookloaderdialog.h \
        epubrenderer.h \
        exceptions.h \
        helpers.h \
        mainwindow.h

FORMS += \
        bookloader/bookloaderdialog.ui \
        mainwindow.ui

RC_ICONS = app.ico

RESOURCES += \
    main.qrc

# copy assets to build folder
CONFIG += file_copies
webcore.files = embedded_web_resources/
webcore.path = $$DESTDIR/
COPIES += webcore
example_books.files = ../example_books/
example_books.path = $$DESTDIR/
COPIES += example_books

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
