#-------------------------------------------------
#
# Project created by QtCreator 2017-02-24T23:10:29
#
#-------------------------------------------------

QT       += core gui xml network
CONFIG += c++11

# Store temporary files in separate directories
win32 {
    CONFIG(debug, release|debug):OBJECTS_DIR = tmp/debug
    CONFIG(release, release|debug):OBJECTS_DIR = tmp/release
} else {
    OBJECTS_DIR    = tmp
}

INCLUDEPATH += $$(MYHTML_ROOT)/include

linux-g++:QMAKE_TARGET.arch = $$QMAKE_HOST.arch
linux-g++-32:QMAKE_TARGET.arch = x86
linux-g++-64:QMAKE_TARGET.arch = x86_64


CONFIG(release, debug|release) {
    !contains(QMAKE_TARGET.arch, x86_64) {
        message("release x86 build")
        LIBS += -L$$(MYHTML_ROOT)/build/Release
    } else {
        message("release x86_64 build")
        LIBS += -L$$(MYHTML_ROOT)/build/x64/Release
    }
} else {
    !contains(QMAKE_TARGET.arch, x86_64) {
        message("debug x86 build")
        LIBS += -L$$(MYHTML_ROOT)/build/Debug
    } else {
        message("debug x86_64 build")
        LIBS += -L$$(MYHTML_ROOT)/build/x64/Debug
    }
}

win32 {
    LIBS += myhtml_static.lib
} else {
    LIBS += -lmyhtml_static
}

MOC_DIR = $$OBJECTS_DIR/moc
RCC_DIR = $$OBJECTS_DIR/rcc
UI_DIR = $$OBJECTS_DIR/uic


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = fadvisor
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    htmlparser.cpp \
    octave.cpp \
    utils.cpp \
    markstorage.cpp

HEADERS  += mainwindow.h \
    htmlparser.h \
    octave.h \
    utils.h \
    markstorage.h

include(rest/rest.pri)
include(cothinker/cothinker.pri)

FORMS    += mainwindow.ui

win32:RC_FILE = app.rc

win32 {
    CONFIG(debug, release|debug):QMAKE_POST_LINK += windeployqt $$OUT_PWD/debug
    CONFIG(release, release|debug):QMAKE_POST_LINK += windeployqt $$OUT_PWD/release
}

SUBDIRS += tests
RESOURCES += icons.qrc
