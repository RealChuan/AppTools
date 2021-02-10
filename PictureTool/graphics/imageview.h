#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QGraphicsView>

#include "graphics_global.h"

namespace Graphics {

class GraphicsPixmapItem;
class ImageViewPrivate;
class GRAPHICS_EXPORT ImageView : public QGraphicsView
{
    Q_OBJECT
public:
    ImageView(QWidget *parent = nullptr);
    ~ImageView() override;

    QPixmap pixmap() const;
    GraphicsPixmapItem *pixmapItem();

public slots:
    void createScene(const QString &imageUrlChanged);
    void setPixmap(const QPixmap& pixmap);

    void setViewBackground(bool enable);
    void setViewOutline(bool enable);
    void setViewCrossLine(bool enable);
    void zoomIn();
    void zoomOut();
    void resetToOriginalSize();
    void fitToScreen();

    void rotateNinetieth();
    void anti_rotateNinetieth();

signals:
    void scaleFactorChanged(qreal factor);
    void imageSizeChanged(const QSize &size);
    void imageUrlChanged(const QString&);

protected:
    void drawBackground(QPainter *painter, const QRectF &rect) override;
    void drawForeground(QPainter *painter, const QRectF &rect) override;

    void wheelEvent(QWheelEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    void initScene();
    void createPopMenu();
    QRect textRect(const Qt::Corner pos, const QFontMetrics& metrics, const QString& text);
    void drawInfo(QPainter *painter);
    void drawCrossLine(QPainter *painter);
    void emitScaleFactor();
    void doScale(qreal factor);
    void reset();

    QScopedPointer<ImageViewPrivate> d_ptr;
};

}

#endif // IMAGEVIEW_H
