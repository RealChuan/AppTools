QT       += core gui network serialport sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AppTools
TEMPLATE = app

CONFIG += c++11
RC_ICONS = AppTools.ico

DEFINES += QT_MESSAGELOGCONTEXT
DEFINES += QT_DEPRECATED_WARNINGS

# Output directory
CONFIG(debug, debug|release) {
    output = debug
}
CONFIG(release, debug|release) {
    output = release
}

DESTDIR     = $$PWD/bin
OBJECTS_DIR = $$output
MOC_DIR     = $$output
RCC_DIR     = $$output
UI_DIR      = $$output

include(help/help.pri)
include(ui/ui.pri)

SOURCES += \
    main.cpp
