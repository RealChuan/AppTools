include(../Common.pri)

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app

win32 {
LIBS += -L$$APP_OUTPUT_PATH/../libs
}

unix {
LIBS += -L$$APP_OUTPUT_PATH
}

LIBS += \
    -l$$replaceLibName(utils) \
    -l$$replaceLibName(mainwindow)

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    main.cpp

DESTDIR = $$APP_OUTPUT_PATH
