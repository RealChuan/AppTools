#ifndef GRAPHICSCIRCLEITEM_H
#define GRAPHICSCIRCLEITEM_H

#include "basicgraphicsitem.h"

struct GRAPHICS_EXPORT Circle{
    bool isVaild() const;
    QRectF boundingRect() const;

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

    static bool checkCircle(const Circle &circle, const double margin);

    void setCircle(const Circle&);
    Circle circle() const;

    bool isValid() const override;
    int type() const override;
    QPainterPath shape() const override;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;
private:
    void pointsChanged(const QPolygonF &ply);
    void showHoverCircle(const QPolygonF &ply);

    QScopedPointer<GraphicsCircleItemPrivate> d;
};

#endif // GRAPHICSCIRCLEITEM_H
