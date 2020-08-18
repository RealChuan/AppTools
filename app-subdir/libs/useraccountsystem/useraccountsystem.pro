include(../libs.pri)

QT += widgets sql

DEFINES += USERACCOUNTSYSTEM_LIBRARY
TARGET = $$replaceLibName(useraccountsystem)

LIBS += \
    -l$$replaceLibName(utils) \
    -l$$replaceLibName(extensionsystem) \
    -l$$replaceLibName(controls) \

SOURCES += \
    accountquery.cpp \
    changepasswdwidget.cpp \
    currentloginwidget.cpp \
    loginwidget.cpp \
    registerwidget.cpp \
    useraccountsystem.cpp

HEADERS += \
    useraccountsystem_global.h \
    accountquery.h \
    changepasswdwidget.h \
    currentloginwidget.h \
    loginwidget.h \
    registerwidget.h \
    useraccountsystem.h
