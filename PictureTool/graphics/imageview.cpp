#include "imageview.h"
#include "graphicspixmapitem.h"

#include <controls/messbox.h>

#include <QGraphicsPixmapItem>
#include <QWheelEvent>
#include <QtMath>
#include <QMimeData>
#include <QDebug>
#include <QMenu>
#include <QApplication>

using namespace Control;

namespace Graphics {

class ImageViewPrivate{
public:
    ImageViewPrivate(QWidget *parent)
        : owner(parent)
        , menu(new QMenu){
        pixmapItem = new GraphicsPixmapItem;

        // background item
        backgroundItem = new QGraphicsRectItem;
        backgroundItem->setBrush(Qt::white);
        backgroundItem->setPen(Qt::NoPen);
        backgroundItem->setVisible(showBackground);
        backgroundItem->setZValue(-1);

        // outline
        outlineItem = new QGraphicsRectItem;
        QPen outline(Qt::black, 1, Qt::DashLine);
        outline.setCosmetic(true);
        outlineItem->setPen(outline);
        outlineItem->setBrush(Qt::NoBrush);
        outlineItem->setVisible(showOutline);
        outlineItem->setZValue(1);
    }
    QWidget *owner;
    GraphicsPixmapItem *pixmapItem;
    QGraphicsRectItem *backgroundItem;
    QGraphicsRectItem *outlineItem;
    bool showBackground = false;
    bool showOutline = false;
    bool showCrossLine = false;
    QString rgbInfo;
    QPointF mousePoint;
    QScopedPointer<QMenu> menu;

    const qreal scaleFactor = 1.2;
};

ImageView::ImageView(QWidget *parent)
    : QGraphicsView(parent)
    , d_ptr(new ImageViewPrivate(this))
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
    createPopMenu();
}

ImageView::~ImageView()
{
}

QPixmap ImageView::pixmap() const
{
    return d_ptr->pixmapItem->pixmap();
}

GraphicsPixmapItem *ImageView::pixmapItem()
{
    return d_ptr->pixmapItem;
}

void ImageView::createScene(const QString &imageUrl)
{
    qDebug() << imageUrl;
    QImage image(imageUrl);
    if(image.isNull()){
        MessBox::Warning(this,
                         tr("Picture failed to open, Url: %1!").arg(imageUrl),
                         MessBox::CloseButton);
        return;
    }

    QPixmap pixmap = QPixmap::fromImage(image);
    if(pixmap.isNull()){
        MessBox::Warning(this,
                         tr("Picture failed to open, Url: %1!").arg(imageUrl),
                         MessBox::CloseButton);
        return;
    }

    emit imageUrlChanged(imageUrl);

    setPixmap(pixmap);
}

void ImageView::setPixmap(const QPixmap &pixmap)
{
    d_ptr->pixmapItem->setCustomPixmap(pixmap);
    QRectF rectF = d_ptr->pixmapItem->boundingRect();
    d_ptr->backgroundItem->setRect(rectF);
    d_ptr->outlineItem->setRect(rectF);

    scene()->setSceneRect(rectF);
    //resetToOriginalSize();
    fitToScreen();

    emit imageSizeChanged(pixmap.size());
}

void ImageView::setViewBackground(bool enable)
{
    d_ptr->showBackground = enable;
    if (d_ptr->backgroundItem)
        d_ptr->backgroundItem->setVisible(enable);
}

void ImageView::setViewOutline(bool enable)
{
    d_ptr->showOutline = enable;
    if (d_ptr->outlineItem)
        d_ptr->outlineItem->setVisible(enable);
}

void ImageView::setViewCrossLine(bool enable)
{
    d_ptr->showCrossLine = enable;
    if(!enable)
        scene()->update();
}

void ImageView::zoomIn()
{
    doScale(d_ptr->scaleFactor);
}

void ImageView::zoomOut()
{
    doScale(1.0 / d_ptr->scaleFactor);
}

void ImageView::resetToOriginalSize()
{
    resetTransform();
    emitScaleFactor();
}

void ImageView::fitToScreen()
{
    fitInView(d_ptr->pixmapItem, Qt::KeepAspectRatio);
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

    if(d_ptr->showCrossLine && d_ptr->pixmapItem){
        drawInfo(painter);
        drawCrossLine(painter);
    }

    painter->restore();
}

void ImageView::wheelEvent(QWheelEvent *event)
{
    qreal factor = qPow(d_ptr->scaleFactor, event->angleDelta().y() / 240.0);
    doScale(factor);
    event->accept();
}

void ImageView::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);

    if(d_ptr->showCrossLine && d_ptr->pixmapItem){
        QPointF pointF = mapToScene(event->pos());
        d_ptr->mousePoint = event->pos();
        if(d_ptr->pixmapItem->contains(pointF)){
            QRgb rgb = d_ptr->pixmapItem->pixmap().toImage().pixel(pointF.toPoint());
            d_ptr->rgbInfo = QString("( %1, %2 ) | %3 %4 %5")
                                 .arg(QString::number(pointF.x())
                                          , QString::number(pointF.y())
                                          , QString::number(qRed(rgb))
                                          , QString::number(qGreen(rgb))
                                          , QString::number(qBlue(rgb)));
            scene()->update();
        }
    }
}

void ImageView::mouseDoubleClickEvent(QMouseEvent*)
{
    fitToScreen();
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

void ImageView::contextMenuEvent(QContextMenuEvent *event)
{
    d_ptr->menu->exec(event->globalPos());
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

    scene()->addItem(d_ptr->backgroundItem);
    scene()->addItem(d_ptr->pixmapItem);
    scene()->addItem(d_ptr->outlineItem);
}

void ImageView::createPopMenu()
{
    d_ptr->menu->addAction(tr("Original Size"), this, &ImageView::resetToOriginalSize);
    d_ptr->menu->addAction(tr("Adapt To Screen"), this, &ImageView::fitToScreen);
    d_ptr->menu->addAction(tr("Rotate 90 Clockwise"), this, &ImageView::rotateNinetieth);
    d_ptr->menu->addAction(tr("Rotate 90 Counterclockwise"), this, &ImageView::anti_rotateNinetieth);
    d_ptr->menu->addSeparator();

    QAction *showBackgroundAction = new QAction(tr("Show Background"), this);
    showBackgroundAction->setCheckable(true);
    connect(showBackgroundAction, &QAction::triggered, this, &ImageView::setViewBackground);
    QAction *showOutlineAction = new QAction(tr("Show Outline"), this);
    showOutlineAction->setCheckable(true);
    connect(showOutlineAction, &QAction::triggered, this, &ImageView::setViewOutline);
    QAction *showCrossLineAction = new QAction(tr("Show CrossLine"), this);
    showCrossLineAction->setCheckable(true);
    connect(showCrossLineAction, &QAction::triggered, this, &ImageView::setViewCrossLine);
    d_ptr->menu->addAction(showBackgroundAction);
    d_ptr->menu->addAction(showOutlineAction);
    d_ptr->menu->addAction(showCrossLineAction);
}

QRect ImageView::textRect(const Qt::Corner pos, const QFontMetrics &metrics, const QString &text)
{
    int startX = 1;
    int startY = 1;
    int rectWidth = metrics.width(text) + 10;
    int rectHeight = metrics.lineSpacing() + 5;

    switch (pos) {
    case Qt::TopLeftCorner: break;
    case Qt::BottomLeftCorner: startY = height() - rectHeight; break;
    case Qt::TopRightCorner: startX = width() - rectWidth; break;
    case Qt::BottomRightCorner:
        startX = width() - rectWidth;
        startY = height() - rectHeight;
        break;
    }

    return QRect(startX, startY, rectWidth, rectHeight);
}

void ImageView::drawInfo(QPainter *painter)
{
    if(d_ptr->rgbInfo.isEmpty())
        return;
    QFontMetrics metrics = painter->fontMetrics();
    int marginX = 5;
    int marginY = metrics.leading() + metrics.ascent() + 2;
    QRect rect = textRect(Qt::TopRightCorner, metrics, d_ptr->rgbInfo);
    QPoint textPos = QPoint(rect.x() + marginX, rect.y() + marginY);

    painter->setPen(Qt::NoPen);
    QColor bgColor(47, 53, 65);
    bgColor.setAlpha(200);
    painter->setBrush(bgColor);
    painter->drawRect(rect);
    painter->setPen(QColor(83,209,255));
    painter->drawText(textPos, d_ptr->rgbInfo);
}

void ImageView::drawCrossLine(QPainter *painter)
{
    QPen pen = painter->pen();
    pen.setColor(QColor(0, 230, 230));
    pen.setWidthF(0.8);
    painter->setPen(pen);

    int w = rect().width();
    int h = rect().height();

    painter->drawLine(QPointF(0, d_ptr->mousePoint.y()),
                      QPointF(w, d_ptr->mousePoint.y()));
    painter->drawLine(QPointF(d_ptr->mousePoint.x(), 0),
                      QPointF(d_ptr->mousePoint.x(), h));
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
    d_ptr->pixmapItem->setTransformationMode(transform().m11() < 1 ?
                                                                   Qt::SmoothTransformation:
                                                                   Qt::FastTransformation);
}

void ImageView::reset()
{
    scene()->clear();
    resetTransform();
}

}
