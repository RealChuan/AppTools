#include "logasync.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QMutex>
#include <QWaitCondition>
#include <QApplication>
#include <QTextStream>

#define ROLLSIZE 1000*1000*1000

const static int kRollPerSeconds_ = 60*60*24;

class FileUtilPrivate{
public:
    FileUtilPrivate(QObject *parent) : owner(parent){}
    QObject *owner;
    QFile file;
    QTextStream stream;
    qint64 startTime = 0;
    qint64 lastRoll = 0;
    int count = 0;
    qint64 autoDelFileDays = 30;
};

FileUtil::FileUtil(qint64 days, QObject *parent) : QObject(parent)
  , d(new FileUtilPrivate(this))
{
    d->autoDelFileDays = days;
    newDir("log");
    rollFile(0);
}

FileUtil::~FileUtil()
{
    d->stream.flush();
    if(d->file.isOpen()){
        d->file.flush();
        d->file.close();
    }
    delete d;
}

void FileUtil::write(const QString &msg)
{
    if(d->file.size() > ROLLSIZE){
        rollFile(++d->count);
    }else{
        qint64 now = QDateTime::currentSecsSinceEpoch();
        qint64 thisPeriod = now / kRollPerSeconds_ * kRollPerSeconds_;
        if(thisPeriod != d->startTime){
            d->count = 0;
            rollFile(0);
            autoDelFile();
        }
    }

    d->stream << msg;
    //d->file.write(msg.toLocal8Bit().constData());
}

void FileUtil::newDir(const QString &path)
{
    QDir dir;
    if(!dir.exists(path))
        dir.mkdir(path);
}

QString FileUtil::getFileName(qint64* now) const
{
    *now = QDateTime::currentSecsSinceEpoch();
    QString data = QDateTime::fromSecsSinceEpoch(*now).toString("yyyy-MM-dd-hh-mm-ss");
    QString filename = QString("./log/%1.%2.%3.%4.log").arg(qAppName()).
            arg(data).arg(QSysInfo::machineHostName()).arg(qApp->applicationPid());
    return filename;
}

bool FileUtil::rollFile(int count)
{
    qint64 now = 0;
    QString filename = getFileName(&now);
    if(count){
        filename += QString(".%1").arg(count);
    }
    qint64 start = now / kRollPerSeconds_ * kRollPerSeconds_;
    if (now > d->lastRoll)
    {
        d->startTime = start;
        d->lastRoll = now;
        if(d->file.isOpen()){
            d->file.flush();
            d->file.close();
        }
        d->file.setFileName(filename);
        d->file.open(QIODevice::WriteOnly | QIODevice::Append |
                     QIODevice::Unbuffered);
        d->stream.setDevice(&d->file);
        fprintf(stderr, "%s\n", filename.toLocal8Bit().constData());
        return true;
    }
    return false;
}

void FileUtil::autoDelFile()
{
    newDir("log");
    QDir dir("./log/");

    QFileInfoList list = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot, QDir::Time);
    QDateTime cur = QDateTime::currentDateTime();
    QDateTime pre = cur.addDays(-d->autoDelFileDays);

    for(QFileInfo info : list){
        QDateTime birthTime = info.lastModified();
        if(birthTime <= pre)
            dir.remove(info.fileName());
    }
}

#define MAXLEN 1000*1000

static LogAsync *g_LogAsync = nullptr;
static QtMsgType MSGTYPE = QtWarningMsg;

// 消息处理函数
void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    if(type < MSGTYPE) return;

    QString level;
    switch(type)
    {
    case QtDebugMsg:    level = QLatin1String("Debug"); break;
    case QtWarningMsg:  level = QLatin1String("Warning"); break;
    case QtCriticalMsg: level = QLatin1String("Critica"); break;
    case QtFatalMsg:    level = QLatin1String("Fatal"); break;
    case QtInfoMsg:     level = QLatin1String("Info"); break;
    }

    const QDateTime dataTime = QDateTime::currentDateTime();
    const QString dataTimeString = dataTime.toString("yyyy-MM-dd hh:mm:ss.zzz");
    const QString threadId = QString::number(qulonglong(QThread::currentThreadId()));

    QString contexInfo;
    contexInfo = QString("File:(%1) Line:(%2)").arg(
                QString(context.file),
                QString::number(context.line));

    const QString message = QString("%1 %2 [%3] %4 - %5\n")
            .arg(dataTimeString, threadId, level, msg, contexInfo);

    if(g_LogAsync)
        g_LogAsync->appendBuf(message);
    else{
        fprintf(stderr, "%s\n", "g_LogAsync = nullptr");
        fprintf(stderr, "%s", message.toLocal8Bit().constData());
    }
}

class LogAsyncPrivate{
public:
    LogAsyncPrivate(QObject *parent) : owner(parent){}
    QObject *owner;
    volatile bool running = false;
    QString currentBuf;
    QVector<QString> buffers;
    QVector<QString> bufferToWrite;
    QMutex mutex;
    QWaitCondition cond;
    unsigned long flushInterval;
};

LogAsync::LogAsync(unsigned long flushInterval, QObject *parent) : QThread(parent)
  , d(new LogAsyncPrivate(this))
{
    qInstallMessageHandler(messageHandler);
    d->flushInterval = flushInterval;
    d->running = true;
    if(nullptr != g_LogAsync){
        delete  g_LogAsync;
        g_LogAsync = nullptr;
    }
    g_LogAsync = this;
}

LogAsync::~LogAsync()
{
    d->running = false;
    if(isRunning()){
        quit();
        wait();
    }
    g_LogAsync = nullptr;
    qInstallMessageHandler(nullptr);
    delete d;
    fprintf(stderr, "%s\n", "~LogAsync");
}

void LogAsync::appendBuf(const QString &buf)
{
    QMutexLocker lock(&d->mutex);
    if(d->currentBuf.size() + buf.size() <= MAXLEN){
        d->currentBuf.append(buf);
    }else{
        d->buffers.push_back(d->currentBuf);
        d->currentBuf.clear();
        d->currentBuf.append(buf);
        d->cond.wakeOne();
    }
}

void LogAsync::setLogLevel(QtMsgType type)
{
    MSGTYPE = type;
}

void LogAsync::run()
{
    FileUtil fileUtil;
    while(d->running){
        {
            QMutexLocker lock(&d->mutex);
            if(d->buffers.isEmpty())
                d->cond.wait(&d->mutex, d->flushInterval);
            d->buffers.push_back(d->currentBuf);
            d->currentBuf.clear();
            d->bufferToWrite.swap(d->buffers);
            d->buffers.clear();
        }
        if(d->bufferToWrite.isEmpty()) continue;
        if(d->bufferToWrite.size() > 25){
            d->bufferToWrite.erase(d->bufferToWrite.begin() + 2, d->bufferToWrite.end());
        }
        for(const QString& buf: d->bufferToWrite){
            fileUtil.write(buf);        //输出到文件
        }
        for(int i=0; i < d->bufferToWrite.size(); i++)
            d->bufferToWrite.pop_back();
        d->bufferToWrite.clear();
    }
}

