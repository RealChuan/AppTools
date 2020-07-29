#ifndef GRAPHICSARCITEM_H
#define GRAPHICSARCITEM_H

#include "basicgraphicsitem.h"

struct GRAPHICS_EXPORT Arch{
    bool isValid() const;
    double diffLen() const;

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
    GraphicsArcItem(const Arch &arch, QGraphicsItem* parent = nullptr);
    ~GraphicsArcItem() override;

    void setArc(const Arch &arch);
    Arch arch() const;

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
    void pointsChanged(const QPolygonF& pts);
    void calculateArch(const QPolygonF& pts);
    bool calculateCurrentPath(const QPolygonF &pts);

    QPainterPath calculatePath(const QPolygonF &pts, QPainterPath &shape);
    bool ptsAndPathVaild(const QPainterPath& path, const QPolygonF& pts);

private:
    QScopedPointer<GraphicsArcItemPrivate> d;
};

#endif // GRAPHICSARCITEM_H
