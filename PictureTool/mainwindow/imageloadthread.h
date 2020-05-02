#ifndef IMAGELOADTHREAD_H
#define IMAGELOADTHREAD_H

#include <QThread>

class ImageLoadThreadPrivate;
class ImageLoadThread : public QThread
{
    Q_OBJECT
public:
    ImageLoadThread(int width, const QString&, QObject *parent = nullptr);
    ~ImageLoadThread() override;

signals:
    void imageReady(const QString&, const QString&, const QImage&);

protected:
    void run() override;

private:
    QScopedPointer<ImageLoadThreadPrivate> d;
};

#endif // IMAGELOADTHREAD_H
