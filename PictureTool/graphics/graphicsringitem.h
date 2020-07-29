#ifndef GRAPHICSRINGITEM_H
#define GRAPHICSRINGITEM_H

#include "basicgraphicsitem.h"

struct GRAPHICS_EXPORT Ring{
    QRectF boundingRect() const;
    QRectF minBoundingRect() const;
    bool isVaild() const;

    QPointF center;
    double minRadius = 0;
    double maxRadius = 0;
};

struct GraphicsRingItemPrivate;
class GRAPHICS_EXPORT GraphicsRingItem : public BasicGraphicsItem
{
public:
    enum MouseRegion{
        InEdge0,
        InEdge1,
        None
    };
    GraphicsRingItem(QGraphicsItem *parent = nullptr);
    GraphicsRingItem(const Ring &ring, QGraphicsItem* parent = nullptr);
    ~GraphicsRingItem() override;

    void setRing(const Ring&);
    Ring ring() const;

    bool isValid() const override;
    int type() const override;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;
private:
    void computeCache(const Ring& ring, QPolygonF& pts);
    bool checkMaxCircleValid(const QPolygonF& pts, QPointF& center, double& radius);
    bool checkMinCircleValid(const QPolygonF& pts);
    void showRingFromCache();

    QScopedPointer<GraphicsRingItemPrivate> d;
};

#endif // GRAPHICSRINGITEM_H
