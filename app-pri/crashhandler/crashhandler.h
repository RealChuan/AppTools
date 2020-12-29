#ifndef CRASHHANDLER_H
#define CRASHHANDLER_H

#include <QtCore>

#if defined(Q_OS_WIN32)
#include <qt_windows.h>
LONG WINAPI UnhandledExceptionFilterEx(LPEXCEPTION_POINTERS lpExceptionInfo);
#endif

namespace Utils {

void openCrashAndLogPath();

void setCrashHandler();

}

#endif // CRASHHANDLER_H
