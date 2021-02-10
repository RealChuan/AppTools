include(../libs.pri)

QT += widgets

DEFINES += GRAPHICS_LIBRARY
TARGET = $$replaceLibName(graphics)

LIBS += \
    -l$$replaceLibName(controls) \

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
    basicgraphicsitem.cpp \
    graphics.cpp \
    graphicsarcitem.cpp \
    graphicscircleitem.cpp \
    graphicslineitem.cpp \
    graphicspixmapitem.cpp \
    graphicspolygonitem.cpp \
    graphicsrectitem.cpp \
    graphicsringitem.cpp \
    graphicsrotatedrectitem.cpp \
    imageview.cpp

HEADERS += \
    basicgraphicsitem.h \
    graphics.h \
    graphics_global.h \
    graphicsarcitem.h \
    graphicscircleitem.h \
    graphicslineitem.h \
    graphicspixmapitem.h \
    graphicspolygonitem.h \
    graphicsrectitem.h \
    graphicsringitem.h \
    graphicsrotatedrectitem.h \
    imageview.h

RESOURCES +=
