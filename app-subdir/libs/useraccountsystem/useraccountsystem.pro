include(../libs.pri)

QT += widgets sql

DEFINES += USERACCOUNTSYSTEM_LIBRARY
TARGET = $$replaceLibName(useraccountsystem)

win32{
LIBS += \
    -l$$DESTDIR/$$replaceLibName(utils) \
    -l$$DESTDIR/$$replaceLibName(extensionsystem) \
    -l$$DESTDIR/$$replaceLibName(controls) \
}

unix:!macx {
LIBS += \
    -L$$DESTDIR \
    -l$$replaceLibName(utils) \
    -l$$replaceLibName(extensionsystem) \
    -l$$replaceLibName(controls) \
}

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
