#ifndef GRAPHICSPOLYGONITEM_H
#define GRAPHICSPOLYGONITEM_H

#include "basicgraphicsitem.h"

struct GraphicsPolygonItemPrivate;
class GRAPHICS_EXPORT GraphicsPolygonItem : public BasicGraphicsItem
{
public:
    GraphicsPolygonItem(QGraphicsItem *parent = nullptr);
    GraphicsPolygonItem(const QPolygonF &polygon, QGraphicsItem* parent = nullptr);
    ~GraphicsPolygonItem() override;

    void setPolygon(const QPolygonF& ply);
    QPolygonF polygon() const;

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
    void pointsChanged(const QPolygonF &ply);
    void showHoverPolygon(const QPolygonF &ply);

    QScopedPointer<GraphicsPolygonItemPrivate> d;
};

#endif // GRAPHICSPOLYGONITEM_H
