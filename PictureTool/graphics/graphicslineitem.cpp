#include "graphicslineitem.h"

#include <QGraphicsSceneHoverEvent>
#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

struct GraphicsLineItemPrivate{
    QLineF line;
};

GraphicsLineItem::GraphicsLineItem(QGraphicsItem *parent)
    : BasicGraphicsItem(parent)
    , d(new GraphicsLineItemPrivate)
{
}

GraphicsLineItem::GraphicsLineItem(const QLineF& line, QGraphicsItem *parent)
    : BasicGraphicsItem(parent)
    , d(new GraphicsLineItemPrivate)
{
    setLine(line);
}

GraphicsLineItem::~GraphicsLineItem()
{
}

void GraphicsLineItem::setLine(const QLineF& line)
{
    prepareGeometryChange();
    QLineF line_tmp = line;
    if(line_tmp.isNull())
        return;
    d->line = line;
    QPolygonF cache;
    cache << line.p1() << line.p2();
    setCache(cache);
}

QLineF GraphicsLineItem::line() const
{
    return d->line;
}

bool GraphicsLineItem::isValid() const
{
    return !d->line.isNull();
}

int GraphicsLineItem::type() const
{
    return LINE;
}

void GraphicsLineItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() != Qt::LeftButton)
        return BasicGraphicsItem::mousePressEvent(event);

    setClickedPos(event->scenePos());
    QPointF point = event->pos();
    if(!isValid() && scene()->sceneRect().contains(point)){
        QPolygonF pts_tmp = cache();
        if(pts_tmp.size() < 2){
            pts_tmp.append(point);
            pointsChanged(pts_tmp);
        }
    }
}

void GraphicsLineItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if((event->buttons() & Qt::LeftButton) == 0
            || !isValid())
        return;

    if(!isSelected())
        setSelected(true);

    QPointF pos = event->scenePos();
    QPolygonF ply = cache();
    QPointF dp = pos - clickedPos();
    setClickedPos(pos);
    switch (mouseRegion()) {
    case DotRegion:{
        int index = hoveredDotIndex();
        ply.replace(index, pos);
    } break;
    case All: ply.translate(dp); break;
    default: break;
    }
    pointsChanged(ply);
}

void GraphicsLineItem::paint(QPainter *painter,
                             const QStyleOptionGraphicsItem *option,
                             QWidget*)
{
    painter->setRenderHint(QPainter::Antialiasing);
    double linew = 2 * pen().widthF() / painter->transform().m11();
    painter->setPen(QPen(LineColor, linew));
    setMargin(painter->transform().m11());

    if(isValid())
        painter->drawLine(d->line);

    if(option->state & QStyle::State_Selected)
        drawAnchor(painter);
}

void GraphicsLineItem::pointsChanged(QPolygonF &ply)
{
    switch (ply.size()) {
    case 1: setCache(ply); break;
    case 2:
        if(scene()->sceneRect().contains(ply.boundingRect()))
            setLine(QLineF(ply[0], ply[1]));
        break;
    default: break;
    }
    update();
}
