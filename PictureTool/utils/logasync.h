#ifndef LOGASYNC_H
#define LOGASYNC_H

#include <QThread>

#include "utils_global.h"

struct FileUtilPrivate;
class FileUtil : public QObject
{
    Q_OBJECT
public:
    explicit FileUtil(qint64 days = 30, QObject *parent = nullptr);
    ~FileUtil();

public slots:
    void onWrite(const QString&);

private:
    void newDir(const QString &);
    QString getFileName(qint64 *now) const;
    bool rollFile(int);
    void autoDelFile();

    QScopedPointer<FileUtilPrivate> d;
};

class LogAsyncPrivate;
class UTILS_EXPORT LogAsync : public QThread
{
    Q_OBJECT
public:
    static LogAsync* instance();

    void setLogLevel(QtMsgType);    //日志级别
    void startWork();
    void finish();

signals:
    void appendBuf(const QString&);

protected:
    void run() override;

private:
    LogAsync(QObject *parent = nullptr);
    ~LogAsync() override;

    QScopedPointer<LogAsyncPrivate> d;
};

#endif // LOGASYNC_H
