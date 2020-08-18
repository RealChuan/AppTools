include(../libs.pri)

QT += widgets

DEFINES += EXTENSIONSYSTEM_LIBRARY
TARGET = $$replaceLibName(extensionsystem)

LIBS += \
    -l$$replaceLibName(utils) \
    -l$$replaceLibName(aggregation) \

HEADERS += \
    pluginerrorview.h \
    plugindetailsview.h \
    invoker.h \
    iplugin.h \
    iplugin_p.h \
    extensionsystem_global.h \
    pluginmanager.h \
    pluginmanager_p.h \
    pluginspec.h \
    pluginspec_p.h \
    pluginview.h \
    optionsparser.h \
    pluginerroroverview.h

SOURCES += \
    pluginerrorview.cpp \
    plugindetailsview.cpp \
    invoker.cpp \
    iplugin.cpp \
    pluginmanager.cpp \
    pluginspec.cpp \
    pluginview.cpp \
    optionsparser.cpp \
    pluginerroroverview.cpp

FORMS += \
    pluginerrorview.ui \
    plugindetailsview.ui \
    pluginerroroverview.ui

RESOURCES += \
    images.qrc
