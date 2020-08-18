include(../Common.pri)

win32 {
DLLDESTDIR = $$APP_OUTPUT_PATH/test
LIBS += -L$$APP_OUTPUT_PATH/../libs
}

unix {
DESTDIR = $$APP_OUTPUT_PATH/test
LIBS += -L$$APP_OUTPUT_PATH
}
