QT       += core gui network serialport sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AppTools
TEMPLATE = app

CONFIG += c++11
RC_ICONS = AppTools.ico

DEFINES += QT_MESSAGELOGCONTEXT
DEFINES += QT_DEPRECATED_WARNINGS

include(help/help.pri)
include(ui/ui.pri)

SOURCES += \
    main.cpp

contains(QT_ARCH, i386) {
    BIN = bin-32
}else{
    BIN = bin-64
}

CONFIG(debug, debug|release) {
    APP_OUTPUT_PATH = $$PWD/$$BIN/debug
}else{
    APP_OUTPUT_PATH = $$PWD/$$BIN/release
}

DESTDIR = $$APP_OUTPUT_PATH
