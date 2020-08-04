include(../plugins.pri)

QT += widgets

DEFINES += COREPLUGIN_LIBRARY
TARGET = $$replaceLibName(coreplugin)

win32{
LIBS += \
    -l$$DESTDIR/$$replaceLibName(utils) \
    -l$$DESTDIR/$$replaceLibName(extensionsystem) \
    -l$$DESTDIR/$$replaceLibName(controls) \
    -l$$DESTDIR/$$replaceLibName(core) \
    -l$$DESTDIR/$$replaceLibName(UserAccountSystem) \
}

unix:!macx {
LIBS += \
    -L$$DESTDIR/.. \
    -l$$replaceLibName(utils) \
    -l$$replaceLibName(extensionsystem) \
    -l$$replaceLibName(controls) \
    -l$$$$replaceLibName(core) \
    -l$$replaceLibName(useraccountsystem) \
}

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
