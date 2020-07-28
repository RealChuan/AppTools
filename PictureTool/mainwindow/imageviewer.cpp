#include "imageviewer.h"
#include "imagelistmodel.h"
#include "imageloadthread.h"

#include <graphics/imageview.h>
#include <controls/messbox.h>

#include <QtWidgets>
#include <QtConcurrent>

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
    ImageLoadThread *imageLoadThread = nullptr;

    QComboBox *formatBox;
    QComboBox *colorBox;
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
    clearThumbnail();
}

void ImageViewer::onOpenImage()
{
    QString filename = QFileDialog::getOpenFileName(this
                                                    , tr("Open Image")
                                                    , "."
                                                    , tr("Images (*.png *.jpg)"));
    if(filename.isEmpty()) return;
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

    onDestroyImageLoadThread();
    clearThumbnail();

    // 因为有可能在加载过程中中断，开启一个新的路径进行加载，
    // 所以没有使用线程池方式丢入任务 （QtConcurrent::run）
    d->imageLoadThread = new ImageLoadThread(WIDTH, url, this);
    connect(d->imageLoadThread, &ImageLoadThread::finished,
            this, &ImageViewer::onDestroyImageLoadThread);
    connect(d->imageLoadThread, &ImageLoadThread::imageReady,
            this, &ImageViewer::onImageLoaded);
    d->imageLoadThread->start();
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
    if(pixmap.isNull()) return;
    d->imageVector.push_back(new Image{filename, absoluteFilePath, pixmap});
    d->imageListView->setImageVector(d->imageVector);
}

void ImageViewer::onDestroyImageLoadThread()
{
    if(!d->imageLoadThread) return;
    d->imageLoadThread->deleteLater();
    d->imageLoadThread = nullptr;
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

void ImageViewer::clearThumbnail()
{
    if(d->imageVector.isEmpty()) return;
    qDeleteAll(d->imageVector);
    d->imageVector.clear();
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
    QPushButton *resetButton = new QPushButton(tr("Original Size"), this);
    QPushButton *fitToViewButton = new QPushButton(tr("Adapt To Screen"), this);
    QPushButton *zoomInButton = new QPushButton(tr("Zoom In"), this);
    QPushButton *zoomOutButton = new QPushButton(tr("Zoom Out"), this);
    QPushButton *rotateButton = new QPushButton(tr("Rotate 90 Clockwise"), this);
    QPushButton *anti_rotateButton = new QPushButton(tr("Rotate 90 Counterclockwise"), this);

    QCheckBox *showBackgroundBox = new QCheckBox(tr("Show Background"), this);
    QCheckBox *showOutlineBox = new QCheckBox(tr("Show Outline"), this);
    QCheckBox *showCrossLine = new QCheckBox(tr("Show CrossLine"), this);

    openImageButton->setObjectName("BlueButton");
    resetButton->setObjectName("BlueButton");
    fitToViewButton->setObjectName("BlueButton");
    zoomInButton->setObjectName("BlueButton");
    zoomOutButton->setObjectName("BlueButton");
    rotateButton->setObjectName("BlueButton");
    anti_rotateButton->setObjectName("BlueButton");

    connect(openImageButton, &QPushButton::clicked, this, &ImageViewer::onOpenImage);
    connect(resetButton, &QPushButton::clicked, d->imageView, &ImageView::resetToOriginalSize);
    connect(fitToViewButton, &QPushButton::clicked, d->imageView, &ImageView::fitToScreen);
    connect(zoomInButton, &QPushButton::clicked, d->imageView, &ImageView::zoomIn);
    connect(zoomOutButton, &QPushButton::clicked, d->imageView, &ImageView::zoomOut);
    connect(rotateButton, &QPushButton::clicked, d->imageView, &ImageView::rotateNinetieth);
    connect(anti_rotateButton, &QPushButton::clicked, d->imageView, &ImageView::anti_rotateNinetieth);
    connect(showBackgroundBox, &QCheckBox::clicked, d->imageView, &ImageView::setViewBackground);
    connect(showOutlineBox, &QCheckBox::clicked, d->imageView, &ImageView::setViewOutline);
    connect(showCrossLine, &QCheckBox::clicked, d->imageView, &ImageView::setViewCrossLine);

    QGridLayout *controlLayout = new QGridLayout;
    controlLayout->addWidget(openImageButton, 0, 0, 1, 2);
    controlLayout->addWidget(resetButton, 1, 0, 1, 1);
    controlLayout->addWidget(fitToViewButton, 1, 1, 1, 1);
    controlLayout->addWidget(zoomInButton, 2, 0, 1, 1);
    controlLayout->addWidget(zoomOutButton, 2, 1, 1, 1);
    controlLayout->addWidget(rotateButton, 3, 0, 1, 1);
    controlLayout->addWidget(anti_rotateButton, 3, 1, 1, 1);
    controlLayout->addWidget(showBackgroundBox, 4, 0, 1, 1);
    controlLayout->addWidget(showOutlineBox, 4, 1, 1, 1);
    controlLayout->addWidget(showCrossLine, 5, 0, 1, 1);

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
    QVBoxLayout *rightLayout = new QVBoxLayout(widget);
    rightLayout->addLayout(controlLayout);
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
}
