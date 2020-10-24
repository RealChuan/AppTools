#include "imageviewer.h"
#include "imagelistmodel.h"

#include <graphics/imageview.h>
#include <controls/messbox.h>

#include <QtWidgets>
#include <QtConcurrent>

using namespace Graphics;
using namespace Control;

class ImageViewerPrivate{
public:
    ImageViewerPrivate(QWidget *parent)
        : owner(parent){
        imageView = new ImageView(owner);
        imageViewFormat = new ImageView(owner);
        imageUrlLabel = new QLabel("-", owner);
        imageUrlLabel->setWordWrap(true);
        sizeLabel = new QLabel("-", owner);
        scaleLabel = new QLabel("-", owner);
        imageListView = new ImageListView(owner);

        formatBox = new QComboBox(owner);
        colorBox = new QComboBox(owner);
    }
    QWidget *owner;
    ImageView *imageView;
    ImageView *imageViewFormat;
    QLabel *imageUrlLabel;
    QLabel *sizeLabel;
    QLabel *scaleLabel;
    ImageVector imageVector;
    ImageListView *imageListView;

    QComboBox *formatBox;
    QComboBox *colorBox;
    bool runing = true;
    QFuture<void> watcher;
};

ImageViewer::ImageViewer(QWidget *parent)
    : QWidget(parent)
    , d(new ImageViewerPrivate(this))
{
    setupUI();
    buildConnect();
}

ImageViewer::~ImageViewer()
{
    destroyImageLoadThread();
    clearThumbnail();
}

void ImageViewer::onOpenImage()
{
    QString imageFilters(tr("Images (*.bmp *.gif *.jpg *.jpeg *.png *.svg *.tiff *.webp *.icns "
                            "*.bitmap *.graymap *.pixmap *.tga *.xbitmap *.xpixmap)"));
    qDebug() << imageFilters;
    QString path = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).value(0, QDir::homePath());
    QString filename = QFileDialog::getOpenFileName(this, tr("Open Image"), path, imageFilters);
    if(filename.isEmpty())
        return;
    d->imageView->createScene(filename);
}

void ImageViewer::onScaleFactorChanged(qreal factor)
{
    const QString info = QString::number(factor * 100, 'f', 2) + QLatin1Char('%');
    d->scaleLabel->setText(info);
}

void ImageViewer::onImageSizeChanged(const QSize &size)
{
    QString imageSizeText;
    if (size.isValid())
        imageSizeText = QString::fromLatin1("%1x%2").arg(size.width()).arg(size.height());
    d->sizeLabel->setText(imageSizeText);
}

void ImageViewer::onImageChanged(const QString &url)
{
    d->imageUrlLabel->setText(url);

    for(const Image* image: d->imageVector){
        if(image->absoluteFilePath == url){
            QPixmap pixmap(url);
            d->imageView->setPixmap(pixmap);
            return;
        }
    }

    destroyImageLoadThread();
    clearThumbnail();
    startImageLoadThread(url);
}

void ImageViewer::onChangedImage(int index)
{
    QString filename = d->imageVector.at(index)->absoluteFilePath;
    d->imageView->createScene(filename);
}

void ImageViewer::onImageLoaded(const QString &filename,
                                const QString &absoluteFilePath,
                                const QImage &image)
{
    QPixmap pixmap = QPixmap::fromImage(image);
    if(pixmap.isNull())
        return;
    d->imageVector.push_back(new Image{filename, absoluteFilePath, pixmap});
    d->imageListView->setImageVector(d->imageVector);
}

void ImageViewer::onFormatChecked(bool state)
{
    d->formatBox->setVisible(state);
    d->imageViewFormat->setVisible(state);
    d->colorBox->setVisible(state);
}

void ImageViewer::onFormatChanged(const QString &)
{
    QImage::Format format = QImage::Format(d->formatBox->currentData().toInt());
    Qt::ImageConversionFlags flags = Qt::ImageConversionFlags(d->colorBox->currentData().toInt());

    QImage image = d->imageView->pixmap().toImage();
    QPixmap pixmap = QPixmap::fromImage(image.convertToFormat(format, flags));
    if(pixmap.isNull()){
        MessBox::Warning(this, tr("Format Conversion Failed!"), MessBox::CloseButton);
        return;
    }
    d->imageViewFormat->setPixmap(pixmap);
    d->imageViewFormat->fitToScreen();
}

void ImageViewer::startImageLoadThread(const QString &url)
{
    d->runing = true;
    d->watcher = QtConcurrent::run(this, &ImageViewer::imageLoad, url);
}

void ImageViewer::destroyImageLoadThread()
{
    if(d->watcher.isRunning()){
        d->runing = false;
        d->watcher.waitForFinished();
    }
}

void ImageViewer::imageLoad(const QString &fileUrl)
{
    QFileInfo file(fileUrl);
    QFileInfoList list = file.absoluteDir().entryInfoList(QDir::Files
                                                          | QDir::NoDotAndDotDot);

    for(QFileInfo &info : list){
        if(!d->runing)
            break;
        QImage image(info.absoluteFilePath());
        if(image.isNull())
            continue;
        if(image.width() > WIDTH)
            image = image.scaled(WIDTH, WIDTH, Qt::KeepAspectRatio);
        emit imageLoadReady(info.fileName(), info.absoluteFilePath(), image);
    }
}

void ImageViewer::clearThumbnail()
{
    if(d->imageVector.isEmpty())
        return;
    qDeleteAll(d->imageVector);
    d->imageVector.clear();
    d->imageListView->setImageVector(d->imageVector);
}

void ImageViewer::setupUI()
{
    QMetaEnum Conversionflags = QMetaEnum::fromType<Qt::ImageConversionFlags>();
    for(int i=0; i<Conversionflags.keyCount(); i++)
        d->colorBox->addItem(Conversionflags.key(i), Conversionflags.value(i));

    QMetaEnum imageFormat = QMetaEnum::fromType<QImage::Format>();
    for(int i=0; i<imageFormat.keyCount(); i++)
        d->formatBox->addItem(imageFormat.key(i), imageFormat.value(i));

    QWidget *rightWidget = toolWidget();

    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(d->imageView);
    splitter->addWidget(d->imageViewFormat);
    splitter->addWidget(rightWidget);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes(QList<int>() << INT_MAX << INT_MAX << 1);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(splitter);
    layout->addWidget(d->imageListView);

    d->formatBox->hide();
    d->imageViewFormat->hide();
    d->colorBox->hide();
}

QWidget *ImageViewer::toolWidget()
{
    QPushButton *openImageButton = new QPushButton(tr("Open Picture"), this);
    openImageButton->setObjectName("BlueButton");
    connect(openImageButton, &QPushButton::clicked, this, &ImageViewer::onOpenImage);

    QGroupBox *infoBox = new QGroupBox(tr("Image Information"), this);
    QGridLayout *gridLayout = new QGridLayout(infoBox);
    gridLayout->addWidget(new QLabel(tr("Image Size: "), this), 0, 0, 1, 1);
    gridLayout->addWidget(d->sizeLabel, 0, 1, 1, 1);
    gridLayout->addWidget(new QLabel(tr("Scaling Ratio:"), this), 1, 0, 1, 1);
    gridLayout->addWidget(d->scaleLabel, 1, 1, 1, 1);
    gridLayout->addWidget(new QLabel(tr("Image Url: "), this), 2, 0, 1, 1);
    gridLayout->addWidget(d->imageUrlLabel, 2, 1, 1, 1);

    QCheckBox *formatBox = new QCheckBox(tr("Format"), this);
    connect(formatBox, &QCheckBox::clicked, this, &ImageViewer::onFormatChecked);
    QHBoxLayout *formatLayout = new QHBoxLayout;
    formatLayout->addWidget(formatBox);
    formatLayout->addWidget(d->formatBox);
    formatLayout->addWidget(d->colorBox);

    QWidget *widget = new QWidget(this);
    widget->setObjectName("InfoWidget");
    QVBoxLayout *rightLayout = new QVBoxLayout(widget);
    rightLayout->addWidget(openImageButton);
    rightLayout->addWidget(infoBox);
    rightLayout->addLayout(formatLayout);
    rightLayout->addStretch();

    return widget;
}

void ImageViewer::buildConnect()
{
    connect(d->imageView, &ImageView::scaleFactorChanged, this, &ImageViewer::onScaleFactorChanged);
    connect(d->imageView, &ImageView::imageSizeChanged, this, &ImageViewer::onImageSizeChanged);
    connect(d->imageView, &ImageView::imageUrlChanged, this, &ImageViewer::onImageChanged);
    connect(d->imageListView, &ImageListView::changeItem, this, &ImageViewer::onChangedImage);
    connect(d->formatBox, &QComboBox::currentTextChanged, this, &ImageViewer::onFormatChanged);
    connect(d->colorBox, &QComboBox::currentTextChanged, this, &ImageViewer::onFormatChanged);
    connect(this, &ImageViewer::imageLoadReady, this, &ImageViewer::onImageLoaded);
}
