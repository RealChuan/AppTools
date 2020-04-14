#ifndef UTILS_H
#define UTILS_H

#include "utils_global.h"

#define STRDATETIME qPrintable (QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss"))
#define STRDATETIMEMS qPrintable (QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz"))

class QWidget;

namespace Utils{

UTILS_EXPORT QString appPath();
UTILS_EXPORT void setUTF8Code();
UTILS_EXPORT void setQSS();
UTILS_EXPORT void loadFonts();
UTILS_EXPORT void windowCenter(QWidget *window);
UTILS_EXPORT void sleep(int sec);

}


#endif // UTILS_H
