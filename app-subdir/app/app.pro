include(../Common.pri)

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app
TARGET = AppPlugin

win32{
LIBSPATH = $$APP_OUTPUT_PATH/../libs
LIBS += \
    -l$$LIBSPATH$$/$$replaceLibName(utils) \
    -l$$LIBSPATH$$/$$replaceLibName(extensionsystem) \
    -l$$LIBSPATH$$/$$replaceLibName(controls) \
    -l$$LIBSPATH$$/$$replaceLibName(useraccountsystem) \
}

unix:!macx {
LIBS += \
    -L$$APP_OUTPUT_PATH \
    -l$$replaceLibName(utils) \
    -l$$replaceLibName(extensionsystem) \
    -l$$replaceLibName(controls) \
    -l$$replaceLibName(useraccountsystem) \
}

RC_ICONS = app.ico
ICON     = app.icns

SOURCES += \
    main.cpp

OTHER_FILES += \
    app.ico \
    app.icns

DESTDIR = $$APP_OUTPUT_PATH
