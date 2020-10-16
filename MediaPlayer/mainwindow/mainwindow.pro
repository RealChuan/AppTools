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
    playcontrolwidget.cpp \
    playerwidget.cpp \
    playlistmodel.cpp \
    playlistview.cpp

HEADERS += \
    mainwindow.h \
    mainwindow_global.h \
    mediawidget.h \
    playcontrolwidget.h \
    playerwidget.h \
    playlistmodel.h \
    playlistview.h
