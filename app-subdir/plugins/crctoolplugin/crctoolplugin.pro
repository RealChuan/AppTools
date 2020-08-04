include(../plugins.pri)

QT += widgets

DEFINES += CRCTOOLPLUGIN_LIBRARY
TARGET = $$replaceLibName(crctoolplugin)

win32{
LIBS += \
    -l$$DESTDIR/$$replaceLibName(utils) \
    -l$$DESTDIR/$$replaceLibName(extensionsystem) \
    -l$$DESTDIR/$$replaceLibName(core) \
}

unix:!macx {
LIBS += \
    -L$$DESTDIR/.. \
    -l$$replaceLibName(utils) \
    -l$$replaceLibName(extensionsystem) \
    -l$$replaceLibName(core) \
}

SOURCES += \
    crcpage.cpp \
    crctoolplugin.cpp \
    crcwidget.cpp \
    floatbox.cpp \
    systemconversionbox.cpp

HEADERS += \
    CRCToolPlugin_global.h \
    crcpage.h \
    crctoolplugin.h \
    crcwidget.h \
    floatbox.h \
    systemconversionbox.h

OTHER_FILES += crctoolplugin.json
