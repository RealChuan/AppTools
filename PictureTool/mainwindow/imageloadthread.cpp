#include "imageloadthread.h"

#include <QFileInfo>
#include <QImage>
#include <QDir>
#include <QDebug>

class ImageLoadThreadPrivate{
public:
    ImageLoadThreadPrivate(QThread *parent)
        : owner(parent){}
    QThread *owner;
    QString fileUrl;
    int width;
    volatile bool running = true;
};

ImageLoadThread::ImageLoadThread(int width, const QString &fileUrl, QObject *parent)
    : QThread(parent)
    , d(new ImageLoadThreadPrivate(this))
{
    d->width = width;
    d->fileUrl = fileUrl;
}

ImageLoadThread::~ImageLoadThread()
{
    d->running = false;
    if(isRunning()){
        quit();
        wait();
    }
    qDebug() << "~ImageLoadThread";
}

void ImageLoadThread::run()
{
    QFileInfo file(d->fileUrl);
    QFileInfoList list = file.absoluteDir().entryInfoList(QDir::Files
                                                          | QDir::NoDotAndDotDot);

    for(QFileInfo info : list){
        if(!d->running) break;
        QImage image(info.absoluteFilePath());
        if(image.isNull()) continue;
        if(image.width() > d->width)
            image = image.scaled(d->width, d->width, Qt::KeepAspectRatio);
        emit imageReady(info.fileName(), info.absoluteFilePath(), image);
    }
}
