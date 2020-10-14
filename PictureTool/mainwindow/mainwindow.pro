include(../libs.pri)

QT += widgets concurrent

DEFINES += MAINWINDOW_LIBRARY
TARGET = $$replaceLibName(mainwindow)

LIBS += \
    -l$$replaceLibName(utils) \
    -l$$replaceLibName(controls) \
    -l$$replaceLibName(graphics)

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    drawwidget.cpp \
    imagelistmodel.cpp \
    imageviewer.cpp \
    mainwindow.cpp

HEADERS += \
    drawwidget.h \
    imagelistmodel.h \
    imageviewer.h \
    mainwindow_global.h \
    mainwindow.h
