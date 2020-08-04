include(../tests.pri)

QT += widgets

TEMPLATE = lib

DEFINES += PAGEPLUGIN_LIBRARY
TARGET = $$replaceLibName(pageplugin)

win32{
LIBSPATH = $$APP_OUTPUT_PATH/../libs
LIBS += \
    -l$$LIBSPATH$$/$$replaceLibName(extensionsystem) \
    -l$$LIBSPATH$$/$$replaceLibName(core) \
    -l$$LIBSPATH$$/$$replaceLibName(utils) \
    -l$$LIBSPATH$$/$$replaceLibName(controls) \
}

unix:!macx {
LIBS += \
    -L$$APP_OUTPUT_PATH \
    -l$$replaceLibName(utils) \
    -l$$replaceLibName(extensionsystem) \
    -l$$replaceLibName(controls) \
    -l$$replaceLibName(core) \
}

SOURCES += \
    labelframe.cpp \
    labelpage.cpp \
    pageplugin.cpp

HEADERS += \
    labelframe.h \
    labelpage.h \
    pageplugin.h

OTHER_FILES += pageplugin.json
