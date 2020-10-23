include(../plugins.pri)

QT += widgets

DEFINES += COREPLUGIN_LIBRARY
TARGET = $$replaceLibName(coreplugin)

LIBS += \
    -l$$replaceLibName(utils) \
    -l$$replaceLibName(extensionsystem) \
    -l$$replaceLibName(controls) \
    -l$$$$replaceLibName(core) \
    -l$$replaceLibName(useraccountsystem) \

SOURCES += \
    configwidget.cpp \
    coreplugin.cpp \
    mainwindow.cpp \
    plugindialog.cpp

HEADERS += \
    configwidget.h \
    coreplugin.h \
    mainwindow.h \
    plugindialog.h

OTHER_FILES += coreplugin.json

RESOURCES += \
    icon.qrc
