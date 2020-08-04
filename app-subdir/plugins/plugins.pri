include(../Common.pri)

TEMPLATE = lib

win32{
DESTDIR = $$APP_OUTPUT_PATH/../libs
DLLDESTDIR = $$APP_OUTPUT_PATH/plugins
}

unix:!macx {
DESTDIR = $$APP_OUTPUT_PATH/plugins
}
