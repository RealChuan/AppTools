include(../Common.pri)

win32{
DLLDESTDIR = $$APP_OUTPUT_PATH/test
}

unix:!macx {
DESTDIR = $$APP_OUTPUT_PATH/test
}
