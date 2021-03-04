QT       += core gui network serialport sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

#Release编译,生成*.pdb调试文件
QMAKE_CXXFLAGS_RELEASE += $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE += $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO

include(utils/utils.pri)
include(controls/controls.pri)
include(useraccountsystem/useraccountsystem.pri)
include(tools/tools.pri)
include(crashhandler/crashhandler.pri)
include(mainwindow/mainwindow.pri)

RC_ICONS = app.ico

SOURCES += \
    main.cpp \

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

INCLUDEPATH += $$PWD/
DEPENDPATH  += $$PWD/

TRANSLATIONS += \
    translations/language.zh_cn.ts \
    translations/language.zh_en.ts
