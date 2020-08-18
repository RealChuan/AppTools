#ifndef GRAPHICSARCITEM_H
#define GRAPHICSARCITEM_H

#include "basicgraphicsitem.h"

namespace Graphics {

struct GRAPHICS_EXPORT Arc{
    bool isValid() const;

    QPointF center;
    double minRadius = 0;
    double maxRadius = 0;
    double startAngle = 0;
    double endAngle = 0;
};

struct GraphicsArcItemPrivate;
class GRAPHICS_EXPORT GraphicsArcItem : public BasicGraphicsItem
{
public:
    enum MouseRegion{
        InEdge0,
        InEdge1,
        None,
        InEdgeL,
        InEdgeH
    };

    GraphicsArcItem(QGraphicsItem *parent = nullptr);
    GraphicsArcItem(const Arc &arc, QGraphicsItem* parent = nullptr);
    ~GraphicsArcItem() override;

    void setArc(const Arc &arc);
    Arc arch() const;

    bool isValid() const override;
    int type() const override;
    QRectF boundingRect() const override;
    QPainterPath shape() const override;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;
private:
    void pointsChanged(const QPolygonF& ply);
    void showHoverArc(const QPolygonF &ply);

private:
    QScopedPointer<GraphicsArcItemPrivate> d;
};

}

#endif // GRAPHICSARCITEM_H
