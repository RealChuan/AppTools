#include "basicgraphicsitem.h"

#include <QPen>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QCursor>
#include <QPainter>

struct BasicGraphicsItemPrivate{
    QString name;
    BasicGraphicsItem::MouseRegion mouseRegin = BasicGraphicsItem::None;
    int hoveredDotIndex = -1;
    QPointF clickedPos;
    QPolygonF cache;
    double margin = 25;
};

BasicGraphicsItem::BasicGraphicsItem(QGraphicsItem *parent)
    : QObject(nullptr)
    , QAbstractGraphicsShapeItem(parent)
    , d(new BasicGraphicsItemPrivate)
{
    setPen(QPen(Qt::blue));
    setFlags(ItemIsSelectable
             | ItemIsMovable
             | ItemSendsGeometryChanges
             | ItemIsFocusable );
    setAcceptHoverEvents(true);
}

BasicGraphicsItem::~BasicGraphicsItem()
{

}

QRectF BasicGraphicsItem::boundingRect() const
{
    if(!itemValid())
        return scene()->sceneRect();

    QRectF rectF = d->cache.boundingRect();
    double addLen = d->margin / 2;
    rectF.adjust(-addLen, -addLen, addLen, addLen);
    return rectF;
}

void BasicGraphicsItem::setName(const QString &name)
{
    d->name = name;
}

QString BasicGraphicsItem::name() const
{
    return d->name;
}

void BasicGraphicsItem::setMargin(double m11)
{
    d->margin = 2.0 *  10 / m11;
    if(d->margin <= 3)
        d->margin = 3;

    if(d->margin >= 100)
        d->margin = 100;
}

double BasicGraphicsItem::margin() const
{
    return d->margin;
}

void BasicGraphicsItem::setItemEditable(bool editable)
{
    setAcceptHoverEvents(editable);
}

QVariant BasicGraphicsItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene()) {
        // value is the new position.
        QPointF newPos = value.toPointF();
        QRectF rect = scene()->sceneRect();
        if (!rect.contains(newPos)) {
            // Keep the item inside the scene rect.
            newPos.setX(qMin(rect.right(), qMax(newPos.x(), rect.left())));
            newPos.setY(qMin(rect.bottom(), qMax(newPos.y(), rect.top())));
            return newPos;
        }
    }
    return QGraphicsItem::itemChange(change, value);
}

void BasicGraphicsItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    if(!itemValid())
        return;

    QPointF pos = event->scenePos();
    for(QPointF p : d->cache){
        QPointF m(margin() / 2, margin() / 2);
        QRectF area(p - m, p + m);
        if(area.contains(pos)){
            d->hoveredDotIndex = d->cache.indexOf(p);
            d->mouseRegin = DotRegion;
            setCursor(Qt::PointingHandCursor);
            return;
        }
    }

    d->hoveredDotIndex = -1;
    d->mouseRegin = All;
    setCursor(Qt::SizeAllCursor);
}

void BasicGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    unsetCursor();
    d->mouseRegin = None;
    QAbstractGraphicsShapeItem::hoverLeaveEvent(event);
}

void BasicGraphicsItem::setCache(const QPolygonF &cache)
{
    d->cache = cache;
}

QPolygonF BasicGraphicsItem::cache() const
{
    return d->cache;
}

void BasicGraphicsItem::setClickedPos(const QPointF &pointF)
{
    d->clickedPos = pointF;
}

QPointF BasicGraphicsItem::clickedPos() const
{
    return d->clickedPos;
}

BasicGraphicsItem::MouseRegion BasicGraphicsItem::mouseRegion() const
{
    return d->mouseRegin;
}

void BasicGraphicsItem::setMouseRegion(const BasicGraphicsItem::MouseRegion mouseRegin)
{
    d->mouseRegin = mouseRegin;
}

int BasicGraphicsItem::hoveredDotIndex() const
{
    return d->hoveredDotIndex;
}

void BasicGraphicsItem::drawAnchor(QPainter *painter)
{
    if(!acceptHoverEvents())
        return;

    for(QPointF p : cache())
        painter->fillRect(QRectF(p.x() - d->margin / 2,
                                 p.y() - d->margin / 2,
                                 d->margin,
                                 d->margin),
                          QColor(222,22,222));
}