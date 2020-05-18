#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QWidget>

class ImageViewerPrivate;
class ImageViewer : public QWidget
{
    Q_OBJECT
public:
    explicit ImageViewer(QWidget *parent = nullptr);
    ~ImageViewer();

signals:
    void imageReady(const QImage&);

private slots:
    void onOpenImage();
    void onScaleFactorChanged(qreal factor);
    void onImageSizeChanged(const QSize &size);
    void onImageChanged(const QString&);
    void onChangedImage(int);
    void onImageLoaded(const QString&, const QString&, const QImage&);
    void onDestroyImageLoadThread();

    void onFormatChecked(bool);
    void onFormatChanged(const QString&);
    
private:
    void clearThumbnail();
    void setupUI();
    QWidget *toolWidget();
    void buildConnect();
    
    QScopedPointer<ImageViewerPrivate> d;
};

#endif // IMAGEVIEWER_H
