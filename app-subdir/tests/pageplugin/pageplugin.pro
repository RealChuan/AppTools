include(../tests.pri)

QT += widgets

TEMPLATE = lib

DEFINES += PAGEPLUGIN_LIBRARY
TARGET = $$replaceLibName(pageplugin)

LIBS += \
    -l$$replaceLibName(utils) \
    -l$$replaceLibName(extensionsystem) \
    -l$$replaceLibName(controls) \
    -l$$replaceLibName(core) \

SOURCES += \
    labelframe.cpp \
    labelpage.cpp \
    pageplugin.cpp

HEADERS += \
    labelframe.h \
    labelpage.h \
    pageplugin.h

OTHER_FILES += pageplugin.json
