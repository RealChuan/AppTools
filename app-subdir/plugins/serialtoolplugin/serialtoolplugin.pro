include(../plugins.pri)

QT += widgets serialport

DEFINES += SERIALTOOLPLUGIN_LIBRARY
TARGET = $$replaceLibName(serialtoolplugin)

win32{
LIBS += \
    -l$$DESTDIR/$$replaceLibName(utils) \
    -l$$DESTDIR/$$replaceLibName(extensionsystem) \
    -l$$DESTDIR/$$replaceLibName(core) \
    -l$$DESTDIR/$$replaceLibName(controls) \
}

unix:!macx {
LIBS += \
    -L$$DESTDIR/.. \
    -l$$replaceLibName(utils) \
    -l$$replaceLibName(extensionsystem) \
    -l$$replaceLibName(controls) \
    -l$$replaceLibName(core) \
}

SOURCES += \
    serialpage.cpp \
    serialport.cpp \
    serialportthread.cpp \
    serialtoolplugin.cpp \
    serialwidget.cpp

HEADERS += \
    serialpage.h \
    serialparam.h \
    serialport.h \
    serialportthread.h \
    serialtoolplugin.h \
    serialwidget.h

OTHER_FILES += serialtoolplugin.json
