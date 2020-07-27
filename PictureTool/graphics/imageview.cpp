#include "imageview.h"

#include <controls/messbox.h>

#include <QGraphicsPixmapItem>
#include <QWheelEvent>
#include <QtMath>
#include <QMimeData>
#include <QMessageBox>
#include <QDebug>

const qreal DEFAULT_SCALE_FACTOR = 1.2;

class ImageViewPrivate{
public:
    ImageViewPrivate(QWidget *parent) : owner(parent){
        imageItem = new QGraphicsPixmapItem;
        imageItem->setCacheMode(QGraphicsItem::NoCache);
        imageItem->setZValue(0);

        // background item
        backgroundItem = new QGraphicsRectItem();
        backgroundItem->setBrush(Qt::white);
        backgroundItem->setPen(Qt::NoPen);
        backgroundItem->setVisible(showBackground);
        backgroundItem->setZValue(-1);

        // outline
        outlineItem = new QGraphicsRectItem();
        QPen outline(Qt::black, 1, Qt::DashLine);
        outline.setCosmetic(true);
        outlineItem->setPen(outline);
        outlineItem->setBrush(Qt::NoBrush);
        outlineItem->setVisible(showOutline);
        outlineItem->setZValue(1);
    }
    QWidget *owner;
    QGraphicsPixmapItem *imageItem = nullptr;
    QGraphicsRectItem *backgroundItem = nullptr;
    QGraphicsRectItem *outlineItem = nullptr;
    bool showBackground = false;
    bool showOutline = false;
    bool showCrossLine = false;
    QString rgbInfo;
    QPointF mousePoint;
};

ImageView::ImageView(QWidget *parent) : QGraphicsView(parent)
  , d(new ImageViewPrivate(this))
{
    setScene(new QGraphicsScene(this));
    setTransformationAnchor(AnchorUnderMouse);
    //setDragMode(ScrollHandDrag);
    setViewportUpdateMode(FullViewportUpdate);
    setFrameShape(QFrame::NoFrame);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setCursor(Qt::CrossCursor);
    setMouseTracking(true);
    setAcceptDrops(true);

    initScene();
}

ImageView::~ImageView()
{
}

QPixmap ImageView::pixmap() const
{
    return d->imageItem->pixmap();
}

void ImageView::createScene(const QString &imageUrl)
{
    QImage image(imageUrl);
    if(image.isNull()){
        MessBox::Warning(this, tr("Picture failed to open, Url: %1!").
                         arg(imageUrl), MessBox::CloseButton);
        return;
    }

    QPixmap pixmap = QPixmap::fromImage(image);
    if(pixmap.isNull()){
        MessBox::Warning(this, tr("Picture failed to open, Url: %1!").
                         arg(imageUrl), MessBox::CloseButton);
        return;
    }

    emit imageUrlChanged(imageUrl);

    setPixmap(pixmap);
}

void ImageView::setPixmap(const QPixmap &pixmap)
{
    d->imageItem->setPixmap(pixmap);
    QRectF rectF = d->imageItem->boundingRect();
    d->backgroundItem->setRect(rectF);
    d->outlineItem->setRect(rectF);

    scene()->setSceneRect(rectF);
    resetToOriginalSize();

    emit imageSizeChanged(d->imageItem->boundingRect().toRect().size());
}

void ImageView::setViewBackground(bool enable)
{
    d->showBackground = enable;
    if (d->backgroundItem)
        d->backgroundItem->setVisible(enable);
}

void ImageView::setViewOutline(bool enable)
{
    d->showOutline = enable;
    if (d->outlineItem)
        d->outlineItem->setVisible(enable);
}

void ImageView::setViewCrossLine(bool enable)
{
    d->showCrossLine = enable;
    if(!enable) scene()->update();
}

void ImageView::zoomIn()
{
    doScale(DEFAULT_SCALE_FACTOR);
}

void ImageView::zoomOut()
{
    doScale(1.0 / DEFAULT_SCALE_FACTOR);
}

void ImageView::resetToOriginalSize()
{
    resetTransform();
    emitScaleFactor();
}

void ImageView::fitToScreen()
{
    fitInView(d->imageItem, Qt::KeepAspectRatio);
    emitScaleFactor();
}

void ImageView::rotateNinetieth()
{
    rotate(90);
}

void ImageView::anti_rotateNinetieth()
{
    rotate(-90);
}

void ImageView::drawBackground(QPainter *p, const QRectF &)
{
    p->save();
    p->resetTransform();
    p->setRenderHint(QPainter::SmoothPixmapTransform, false);
    p->drawTiledPixmap(viewport()->rect(), backgroundBrush().texture());
    p->restore();
}

void ImageView::drawForeground(QPainter *painter, const QRectF &rect)
{
    QGraphicsView::drawForeground(painter, rect);
    painter->save();
    painter->resetTransform();

    if(d->showCrossLine && d->imageItem){
        drawInfo(painter);
        drawCrossLine(painter);
    }

    painter->restore();
}

void ImageView::wheelEvent(QWheelEvent *event)
{
    qreal factor = qPow(DEFAULT_SCALE_FACTOR, event->angleDelta().y() / 240.0);
    doScale(factor);
    event->accept();
}

void ImageView::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);

    if(d->showCrossLine && d->imageItem){
        QPointF pointF = mapToScene(event->pos());
        d->mousePoint = mapToParent(event->pos());
        if(d->imageItem->contains(pointF)){
            QRgb rgb = d->imageItem->pixmap().toImage().pixel(pointF.toPoint());
            d->rgbInfo = QString("( %1, %2 ) | %3 %4 %5").
                    arg(QString::number(pointF.x()),
                        QString::number(pointF.y()),
                        QString::number(qRed(rgb)),
                        QString::number(qGreen(rgb)),
                        QString::number(qBlue(rgb)));
            scene()->update();
        }
    }
}

void ImageView::dragEnterEvent(QDragEnterEvent *event)
{
    QGraphicsView::dragEnterEvent(event);
    event->acceptProposedAction();
}

void ImageView::dragMoveEvent(QDragMoveEvent *event)
{
    QGraphicsView::dragMoveEvent(event);
    event->acceptProposedAction();
}

void ImageView::dropEvent(QDropEvent *event)
{
    QGraphicsView::dropEvent(event);
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty())
        return;

    QString fileName = urls.first().toLocalFile();
    createScene(fileName);
}

void ImageView::initScene()
{
    // Prepare background check-board pattern
    QPixmap tilePixmap(64, 64);
    tilePixmap.fill(Qt::white);
    QPainter tilePainter(&tilePixmap);
    QColor color(220, 220, 220);
    tilePainter.fillRect(0, 0, 0x20, 0x20, color);
    tilePainter.fillRect(0x20, 0x20, 0x20, 0x20, color);
    tilePainter.end();

    setBackgroundBrush(tilePixmap);

    scene()->addItem(d->backgroundItem);
    scene()->addItem(d->imageItem);
    scene()->addItem(d->outlineItem);
}

QRect ImageView::textRect(const Qt::Corner pos, const QFontMetrics &metrics, const QString &text)
{
    int startX = 1;
    int startY = 1;
    int rectWidth = metrics.width(text) + 10;
    int rectHeight = metrics.lineSpacing() + 5;

    switch (pos) {
    case Qt::TopLeftCorner: break;
    case Qt::BottomLeftCorner: startY = this->height() - rectHeight; break;
    case Qt::TopRightCorner: startX = this->width() - rectWidth; break;
    case Qt::BottomRightCorner:
        startX = this->width() - rectWidth;
        startY = this->height() - rectHeight;
        break;
    }

    return QRect(startX, startY, rectWidth, rectHeight);
}

void ImageView::drawInfo(QPainter *painter)
{
    if(d->rgbInfo.isEmpty()) return;
    QFontMetrics metrics = painter->fontMetrics();
    int marginX = 5;
    int marginY = metrics.leading() + metrics.ascent() + 2;
    QRect rect = textRect(Qt::TopRightCorner, metrics, d->rgbInfo);
    QPoint textPos = QPoint(rect.x() + marginX, rect.y() + marginY);

    painter->setPen(Qt::NoPen);
    QColor bgColor(47, 53, 65);
    bgColor.setAlpha(200);
    painter->setBrush(bgColor);
    painter->drawRect(rect);
    painter->setPen(QColor(83,209,255));
    painter->drawText(textPos, d->rgbInfo);

}

void ImageView::drawCrossLine(QPainter *painter)
{
    QPen pen = painter->pen();
    pen.setColor(QColor(0, 230, 230));
    pen.setWidthF(0.8);
    painter->setPen(pen);

    int w = rect().width();
    int h = rect().height();

    painter->drawLine(QPointF(0, d->mousePoint.y()),
                      QPointF(w, d->mousePoint.y()));
    painter->drawLine(QPointF(d->mousePoint.x(), 0), QPointF(d->mousePoint.x(), h));
}

void ImageView::emitScaleFactor()
{
    // get scale factor directly from the transform matrix
    qreal factor = transform().m11();
    emit scaleFactorChanged(factor);
}

void ImageView::doScale(qreal factor)
{
    qreal currentScale = transform().m11();
    qreal newScale = currentScale * factor;
    qreal actualFactor = factor;
    // cap to 0.001 - 1000
    if (newScale > 1000)
        actualFactor = 1000./currentScale;
    else if (newScale < 0.001)
        actualFactor = 0.001/currentScale;

    scale(actualFactor, actualFactor);
    emitScaleFactor();
    d->imageItem->setTransformationMode(transform().m11() < 1 ?
                                            Qt::SmoothTransformation : Qt::FastTransformation);
}

void ImageView::reset()
{
    scene()->clear();
    resetTransform();
}
