#ifndef GRAPHICSRECTITEM_H
#define GRAPHICSRECTITEM_H

#include "basicgraphicsitem.h"

struct GraphicsRectItemPrivate;
class GRAPHICS_EXPORT GraphicsRectItem : public BasicGraphicsItem
{
public:
    GraphicsRectItem(QGraphicsItem *parent = nullptr);
    GraphicsRectItem(const QRectF&, QGraphicsItem *parent = nullptr);
    ~GraphicsRectItem() override;

    void setRect(const QRectF&);
    QRectF rect() const;
    bool isValid() const override;
    int type() const override;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

private:
    bool checkCacheValid(const QPolygonF& pyg);
    void showRectFromCache();

    QScopedPointer<GraphicsRectItemPrivate> d;
};

#endif // GRAPHICSRECTITEM_H
