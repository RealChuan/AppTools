#include "imageviewer.h"
#include "imageview.h"
#include "imagelistmodel.h"
#include "imageloadthread.h"

#include <QtWidgets>
#include <QtConcurrent>

class ImageViewerPrivate{
public:
    ImageViewerPrivate(QWidget *parent): owner(parent){
        imageView = new ImageView(owner);
        imageUrlLabel = new QLabel("-", owner);
        imageUrlLabel->setWordWrap(true);
        sizeLabel = new QLabel("-", owner);
        scaleLabel = new QLabel("-", owner);
        imageListView = new ImageListView(owner);
    }
    QWidget *owner;
    ImageView *imageView;
    QLabel *imageUrlLabel;
    QLabel *sizeLabel;
    QLabel *scaleLabel;
    ImageVector imageVector;
    ImageListView *imageListView;
    ImageLoadThread *imageLoadThread = nullptr;
};

ImageViewer::ImageViewer(QWidget *parent) : QWidget(parent)
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
    QString filename = QFileDialog::getOpenFileName(this, tr("Open Image"),
                                                    ".", tr("Images (*.png *.jpg)"));
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

    if(d->imageLoadThread) delete d->imageLoadThread;
    clearThumbnail();

    d->imageLoadThread = new ImageLoadThread(WIDTH, url, this);
    connect(d->imageLoadThread, &ImageLoadThread::finished, [this]{
        d->imageLoadThread->deleteLater();
        d->imageLoadThread = nullptr;
    });
    connect(d->imageLoadThread, &ImageLoadThread::imageReady, this, &ImageViewer::onImageLoaded);
    d->imageLoadThread->start();
}

void ImageViewer::onChangedImage(int index)
{
    QString filename = d->imageVector.at(index)->absoluteFilePath;
    d->imageView->createScene(filename);
}

void ImageViewer::onImageLoaded(const QString &filename, const QString &absoluteFilePath, const QImage &image)
{
    QPixmap pixmap = QPixmap::fromImage(image);
    if(pixmap.isNull()) return;
    d->imageVector.push_back(new Image{filename, absoluteFilePath, pixmap});
    d->imageListView->setImageVector(d->imageVector);
}

void ImageViewer::clearThumbnail()
{
    if(d->imageVector.isEmpty()) return;
    qDeleteAll(d->imageVector);
    d->imageVector.clear();
}

void ImageViewer::setupUI()
{
    QPushButton *openImageButton = new QPushButton(tr("Open Picture"), this);
    QPushButton *zoomInButton = new QPushButton(tr("Zoom In"), this);
    QPushButton *zoomOutButton = new QPushButton(tr("Zoom Out"), this);
    QPushButton *resetButton = new QPushButton(tr("Original Size"), this);
    QPushButton *fitToViewButton = new QPushButton(tr("Adapt To Screen"), this);
    QCheckBox *showBackgroundBox = new QCheckBox(tr("Show Background"), this);
    QCheckBox *showOutlineBox = new QCheckBox(tr("Show Outline"), this);
    QCheckBox *showCrossLine = new QCheckBox(tr("Show CrossLine"), this);

    openImageButton->setObjectName("BlueButton");
    zoomInButton->setObjectName("BlueButton");
    zoomOutButton->setObjectName("BlueButton");
    resetButton->setObjectName("BlueButton");
    fitToViewButton->setObjectName("BlueButton");

    connect(openImageButton, &QPushButton::clicked, this, &ImageViewer::onOpenImage);
    connect(zoomInButton, &QPushButton::clicked, d->imageView, &ImageView::zoomIn);
    connect(zoomOutButton, &QPushButton::clicked, d->imageView, &ImageView::zoomOut);
    connect(resetButton, &QPushButton::clicked, d->imageView, &ImageView::resetToOriginalSize);
    connect(fitToViewButton, &QPushButton::clicked, d->imageView, &ImageView::fitToScreen);
    connect(showBackgroundBox, &QCheckBox::clicked, d->imageView, &ImageView::setViewBackground);
    connect(showOutlineBox, &QCheckBox::clicked, d->imageView, &ImageView::setViewOutline);
    connect(showCrossLine, &QCheckBox::clicked, d->imageView, &ImageView::setViewCrossLine);

    QGridLayout *controlLayout = new QGridLayout;
    controlLayout->addWidget(openImageButton, 0, 0, 1, 2);
    controlLayout->addWidget(resetButton, 1, 0, 1, 1);
    controlLayout->addWidget(fitToViewButton, 1, 1, 1, 1);
    controlLayout->addWidget(zoomInButton, 2, 0, 1, 1);
    controlLayout->addWidget(zoomOutButton, 2, 1, 1, 1);
    controlLayout->addWidget(showBackgroundBox, 3, 0, 1, 1);
    controlLayout->addWidget(showOutlineBox, 3, 1, 1, 1);
    controlLayout->addWidget(showCrossLine, 4, 0, 1, 1);

    QGroupBox *infoBox = new QGroupBox(tr("Image Information"), this);
    QGridLayout *gridLayout = new QGridLayout(infoBox);
    gridLayout->addWidget(new QLabel(tr("Image Size: "), this), 0, 0, 1, 1);
    gridLayout->addWidget(d->sizeLabel, 0, 1, 1, 1);
    gridLayout->addWidget(new QLabel(tr("Scaling Ratio:"), this), 1, 0, 1, 1);
    gridLayout->addWidget(d->scaleLabel, 1, 1, 1, 1);
    gridLayout->addWidget(new QLabel(tr("Image Url: "), this), 2, 0, 1, 1);
    gridLayout->addWidget(d->imageUrlLabel, 2, 1, 1, 1);

    QWidget *rightWidget = new QWidget(this);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightWidget);
    rightLayout->addLayout(controlLayout);
    rightLayout->addWidget(infoBox);
    rightLayout->addStretch();

    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(d->imageView);
    splitter->addWidget(rightWidget);
    splitter->setStretchFactor(0, 1);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(splitter);
    layout->addWidget(d->imageListView);
}

void ImageViewer::buildConnect()
{
    connect(d->imageView, &ImageView::scaleFactorChanged, this, &ImageViewer::onScaleFactorChanged);
    connect(d->imageView, &ImageView::imageSizeChanged, this, &ImageViewer::onImageSizeChanged);
    connect(d->imageView, &ImageView::imageUrlChanged, this, &ImageViewer::onImageChanged);
    connect(d->imageListView, &ImageListView::changeItem, this, &ImageViewer::onChangedImage);
}
