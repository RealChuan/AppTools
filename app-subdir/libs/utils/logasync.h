#ifndef LOGASYNC_H
#define LOGASYNC_H

#include <QThread>

#include "utils_global.h"

class FileUtilPrivate;
class FileUtil : public QObject
{
    Q_OBJECT
public:
    explicit FileUtil(qint64 days = 30, QObject *parent = nullptr);
    ~FileUtil();

    void write(const QString&);

signals:

private:
    void newDir(const QString &);
    QString getFileName(qint64 *now) const;
    bool rollFile(int);
    void autoDelFile();

    FileUtilPrivate *d;
};

class LogAsyncPrivate;
class UTILS_EXPORT LogAsync : public QThread
{
    Q_OBJECT
public:
    LogAsync(unsigned long flushInterval = 5000, QObject *parent = nullptr);
    ~LogAsync() override;

    void appendBuf(const QString&);
    void setLogLevel(QtMsgType);    //日志级别

protected:
    void run() override;

private:
    LogAsyncPrivate *d;
};

#endif // LOGASYNC_H
