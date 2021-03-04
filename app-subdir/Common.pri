CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS

#Release编译,生成*.pdb调试文件
QMAKE_CXXFLAGS_RELEASE += $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE += $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO

contains(QT_ARCH, i386) {
    BIN = bin-32
}else{
    BIN = bin-64
}

CONFIG(debug, debug|release) {
    APP_OUTPUT_PATH = $$PWD/$$BIN/debug
}else{
    APP_OUTPUT_PATH = $$PWD/$$BIN/release
}

INCLUDEPATH += $$PWD/libs/
DEPENDPATH  += $$PWD/libs/

defineReplace(replaceLibName) {
   unset(LIBRARY_NAME)
   LIBRARY_NAME = $$1
   CONFIG(debug, debug|release) {
      !debug_and_release|build_pass {
          mac:RET = $$member(LIBRARY_NAME, 0)_debug
              else:win32:RET = $$member(LIBRARY_NAME, 0)d
      }
   }
   isEmpty(RET):RET = $$LIBRARY_NAME
   return($$RET)
}
