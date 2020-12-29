#ifndef UTILS_H
#define UTILS_H

#include <QtCore>

#define TIMEMS qPrintable(QTime::currentTime().toString("HH:mm:ss zzz"))
#define TIME qPrintable(QTime::currentTime().toString("HH:mm:ss"))
#define QDATE qPrintable(QDate::currentDate().toString("yyyy-MM-dd"))
#define QTIME qPrintable(QTime::currentTime().toString("HH-mm-ss"))
#define DATETIME qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
#define STRDATETIME qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss"))
#define STRDATETIMEMS qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz"))

#define AppPath qApp->applicationDirPath()

#define ConfigFile "./config/config.ini"

class QWidget;
class QSettings;

namespace Utils{

enum Language{
    Chinese,
    English
};

QSettings* settings();
void printBuildInfo();
void setHighDpiEnvironmentVariable();
void setUTF8Code();
void setQSS();
void loadFonts();
void windowCenter(QWidget *window);
void msleep(int msec);
void reboot();
void saveLanguage(Language);
void loadLanguage();
Language getCurrentLanguage();
bool createPath(const QString& path);

}


#endif // UTILS_H
