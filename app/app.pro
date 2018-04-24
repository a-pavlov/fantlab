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
    markstorage.cpp

HEADERS  += mainwindow.h \
    htmlparser.h \
    octave.h \
    markstorage.h

include(rest/rest.pri)
include(cothinker/cothinker.pri)
include(statusbar/statusbar.pri)
include(misc/misc.pri)

FORMS    += mainwindow.ui

win32:RC_FILE = app.rc

win32 {
    CONFIG(debug, release|debug):QMAKE_POST_LINK += windeployqt $$OUT_PWD/debug
    CONFIG(release, release|debug):QMAKE_POST_LINK += windeployqt $$OUT_PWD/release
    DEFINES += UNICODE
    DEFINES += _UNICODE
    DEFINES += WIN32
    DEFINES += _WIN32
    DEFINES += WIN32_LEAN_AND_MEAN
    DEFINES += _WIN32_WINNT=0x0501
    DEFINES += _WIN32_IE=0x0501
    DEFINES += _CRT_SECURE_NO_DEPRECATE
    DEFINES += _SCL_SECURE_NO_DEPRECATE
    DEFINES += _FILE_OFFSET_BITS=64

    contains(QMAKE_HOST.arch, x86) {
        # i686 arch requires frame pointer preservation
        QMAKE_CXXFLAGS_RELEASE += -Oy-
        QMAKE_CXXFLAGS_DEBUG += -Oy-
    }
    release {
        QMAKE_CXXFLAGS_RELEASE += -Zi
        QMAKE_LFLAGS += "/DEBUG"
    }
    LIBS += dbghelp.lib
}

SUBDIRS += tests
RESOURCES += icons.qrc
