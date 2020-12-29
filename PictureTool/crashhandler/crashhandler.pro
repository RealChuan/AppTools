include(../libs.pri)

QT += widgets

DEFINES += CRASHHANDLER_LIBRARY
TARGET = $$replaceLibName(crashhandler)

LIBS += \
	-l$$replaceLibName(utils) \
	-l$$replaceLibName(controls)
	
SOURCES += \
    crashdialog.cpp \
    crashhandler.cpp

HEADERS += \
    crashHandler_global.h \
    crashdialog.h \
    crashhandler.h
