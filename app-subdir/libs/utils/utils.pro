include(../libs.pri)
include(mimetypes/mimetypes.pri)

QT += widgets sql

DEFINES += UTILS_LIBRARY

LIBS += \
    $$APP_OUTPUT_PATH/../libs/controls.lib \

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    benchmarker.cpp \
    categorysortfiltermodel.cpp \
    crcalgorithm.cpp \
    datbasesql.cpp \
    hostosinfo.cpp \
    itemviews.cpp \
    json.cpp \
    qtcassert.cpp \
    stringutils.cpp \
    treemodel.cpp \
    utils.cpp

HEADERS += \
    algorithm.h \
    benchmarker.h \
    categorysortfiltermodel.h \
    crcalgorithm.h \
    datbasesql.h \
    executeondestruction.h \
    hostosinfo.h \
    itemviews.h \
    json.h \
    qtcassert.h \
    stringutils.h \
    treemodel.h \
    utils_global.h \
    utils.h
