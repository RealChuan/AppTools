include(../libs.pri)

QT += widgets multimediawidgets multimedia

DEFINES += MAINWINDOW_LIBRARY
TARGET = $$replaceLibName(mainwindow)

LIBS += \
    -l$$replaceLibName(utils)

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    mainwindow.cpp \
    mediawidget.cpp \
    playcontrols.cpp \
    playerwidget.cpp \
    playlistmodel.cpp \
    playlistwidget.cpp

HEADERS += \
    mainwindow.h \
    mainwindow_global.h \
    mediawidget.h \
    playcontrols.h \
    playerwidget.h \
    playlistmodel.h \
    playlistwidget.h
