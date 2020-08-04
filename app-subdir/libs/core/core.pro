include(../libs.pri)

QT += widgets

DEFINES += CORE_LIBRARY
TARGET = $$replaceLibName(core)

SOURCES += \
    core.cpp \
    mpages.cpp

HEADERS += \
    core_global.h \
    core.h \
    mpages.h
