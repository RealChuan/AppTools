#ifndef GRAPHICSLINEITEM_H
#define GRAPHICSLINEITEM_H

#include "basicgraphicsitem.h"

namespace Graphics {

struct GraphicsLineItemPrivate;
class GRAPHICS_EXPORT GraphicsLineItem : public BasicGraphicsItem
{
public:
    GraphicsLineItem(QGraphicsItem *parent = nullptr);
    GraphicsLineItem(const QLineF& line, QGraphicsItem *parent = nullptr);
    ~GraphicsLineItem() override;

    void setLine(const QLineF& line);
    QLineF line() const;

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
    void showHoverLine(const QPolygonF &ply);

    QScopedPointer<GraphicsLineItemPrivate> d;
};

}

#endif // GRAPHICSLINEITEM_H
