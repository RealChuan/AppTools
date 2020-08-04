include(../Common.pri)

unix:!macx {
CONFIG += c++14
}

TEMPLATE = lib

win32{
DESTDIR = $$APP_OUTPUT_PATH/../libs
DLLDESTDIR = $$APP_OUTPUT_PATH
}

unix:!macx {
DESTDIR = $$APP_OUTPUT_PATH
}
