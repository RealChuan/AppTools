#ifndef GRAPHICSCIRCLEITEM_H
#define GRAPHICSCIRCLEITEM_H

#include "basicgraphicsitem.h"

struct GRAPHICS_EXPORT Circle{
    bool isVaild();
    QRectF boundingRect();

    QPointF center;
    double radius = 0;
};

struct GraphicsCircleItemPrivate;
class GRAPHICS_EXPORT GraphicsCircleItem : public BasicGraphicsItem
{
public:
    GraphicsCircleItem(QGraphicsItem *parent = nullptr);
    GraphicsCircleItem(const Circle&, QGraphicsItem *parent = nullptr);
    ~GraphicsCircleItem() override;

    void setCircle(const Circle&);
    Circle circle() const;
    bool isValid() const override;
    int type() const override;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

private:
    void computeCache(const Circle&, QPolygonF &cache);
    bool checkCacheValid(Circle &circle, const QPolygonF &pyg);
    void showCircleFromCache(const Circle&);

    QScopedPointer<GraphicsCircleItemPrivate> d;
};

#endif // GRAPHICSCIRCLEITEM_H
