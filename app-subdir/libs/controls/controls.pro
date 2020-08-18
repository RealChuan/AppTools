include(../libs.pri)

QT += widgets

DEFINES += CONTROLS_LIBRARY
TARGET = $$replaceLibName(controls)

LIBS += -l$$replaceLibName(utils)

SOURCES += \
    accountcontrols.cpp \
    commonwidget.cpp \
    controls.cpp \
    dialog.cpp \
    messbox.cpp \
    waitwidget.cpp

HEADERS += \
    accountcontrols.h \
    commonwidget.h \
    controls_global.h \
    controls.h \
    dialog.h \
    messbox.h \
    waitwidget.h
