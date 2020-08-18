include(../Common.pri)

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app
TARGET = AppPlugin

win32 {
LIBS += -L$$APP_OUTPUT_PATH/../libs
}

unix {
LIBS += -L$$APP_OUTPUT_PATH
}

LIBS += \
    -l$$replaceLibName(utils) \
    -l$$replaceLibName(extensionsystem) \
    -l$$replaceLibName(controls) \
    -l$$replaceLibName(useraccountsystem) \

RC_ICONS = app.ico
ICON     = app.icns

SOURCES += \
    main.cpp

OTHER_FILES += \
    app.ico \
    app.icns

DESTDIR = $$APP_OUTPUT_PATH
