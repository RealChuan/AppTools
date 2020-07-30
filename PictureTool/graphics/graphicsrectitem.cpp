#include "graphicsrectitem.h"
#include "graphics.h"

#include <QGraphicsSceneHoverEvent>
#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QDebug>

struct GraphicsRectItemPrivate{
    QRectF rect;
    bool linehovered = false;
    QLineF hoveredLine;
};

GraphicsRectItem::GraphicsRectItem(QGraphicsItem *parent)
    : BasicGraphicsItem(parent)
    , d(new GraphicsRectItemPrivate)
{
}

GraphicsRectItem::GraphicsRectItem(const QRectF& rectF, QGraphicsItem *parent)
    : BasicGraphicsItem(parent)
    , d(new GraphicsRectItemPrivate)
{
    setRect(rectF);
}

GraphicsRectItem::~GraphicsRectItem()
{
}

void GraphicsRectItem::setRect(const QRectF& rect)
{
    prepareGeometryChange();
    if(!rect.isValid())
        return;

    d->rect = rect;
    QPolygonF cache;
    cache << rect.topLeft() << rect.bottomRight();
    setCache(cache);
}

QRectF GraphicsRectItem::rect() const
{
    return d->rect;
}

bool GraphicsRectItem::isValid() const
{
    return d->rect.isValid();
}

int GraphicsRectItem::type() const
{
    return RECT;
}

void GraphicsRectItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() != Qt::LeftButton)
        return BasicGraphicsItem::mousePressEvent(event);

    setClickedPos(event->scenePos());
    QPointF p = event->pos();
    if(!isValid() && scene()->sceneRect().contains(p)){
        QPolygonF pts = cache();
        pts.append(event->scenePos());
        if(pts.count() == 2 && !checkCacheValid(pts))
            pts.removeLast();
        setCache(pts);
        showRectFromCache();
    }

    update();
}

QPolygonF polygonFromRect(const QRectF& rect)
{
    QPolygonF ply;
    ply.append(rect.topLeft());
    ply.append(rect.topRight());
    ply.append(rect.bottomRight());
    ply.append(rect.bottomLeft());
    return ply;
}

void GraphicsRectItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if((event->buttons() & Qt::LeftButton) == 0
            || !isValid())
        return;

    if(!isSelected())
        setSelected(true);

    QPointF p = event->scenePos();
    QPolygonF ply = cache();
    QPointF dp = p - clickedPos();
    setClickedPos(p);

    if(d->linehovered){
        QPointF p1 = d->hoveredLine.p1();
        QPointF p2 = d->hoveredLine.p2();

        QPolygonF ply1 = polygonFromRect(rect());
        int index0 = ply1.indexOf(p1);
        int index1 = ply1.indexOf(p2);
        if(index0 < 0 || index1 < 0)
            return;

        if(abs(p1.x() - p2.x()) < 0.0001){ //vertical line
            p1 = p1 + QPointF(dp.x(), 0);
            p2 = p2 + QPointF(dp.x(), 0);
        }else{
            p1 = p1 + QPointF(0, dp.y());
            p2 = p2 + QPointF(0, dp.y());
        }

        ply1.replace(index0, p1);
        ply1.replace(index1, p2);

        d->hoveredLine = QLineF(p1, p2);

        ply.clear();
        ply.append(ply1.at(0));
        ply.append(ply1.at(2));
    }else{
        switch (mouseRegion()) {
        case DotRegion:{
            int index = hoveredDotIndex();
            ply.replace(index, p);
        } break;
        case All: ply.translate(dp); break;
        default: break;
        }
    }
    if(checkCacheValid(ply))
        setCache(ply);
    showRectFromCache();
}

void GraphicsRectItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    showRectFromCache();
    BasicGraphicsItem::mouseReleaseEvent(event);
}

void GraphicsRectItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    if(!isValid())
        return;

    BasicGraphicsItem::hoverMoveEvent(event);
    if(mouseRegion() == DotRegion)
        return;

    QPointF p = event->scenePos();
    QPolygonF ply = polygonFromRect(rect());
    for(int i = 0; i < ply.count(); ++i){
        QLineF pl(ply.at(i), ply.at( (i + 1) % 4));
        QPolygonF tmp = Graphics::boundingFromLine(pl, margin() / 4);
        if(tmp.containsPoint(p, Qt::OddEvenFill)){
            d->linehovered = true;
            d->hoveredLine = pl;
            setCursor(Graphics::curorFromAngle(pl.angle()));
            return;
        }
    }

    d->linehovered = false;
}

void GraphicsRectItem::paint(QPainter *painter,
                             const QStyleOptionGraphicsItem *option,
                             QWidget*)
{
    painter->setRenderHint(QPainter::Antialiasing);
    double linew = 2 * pen().widthF() / painter->transform().m11();
    painter->setPen(QPen(LineColor, linew));
    setMargin(painter->transform().m11());

    QPolygonF pyg = cache();
    if(pyg.size() == 2)
        painter->drawRect(pyg.boundingRect());

    if(option->state & QStyle::State_Selected)
        drawAnchor(painter);
}

bool GraphicsRectItem::checkCacheValid(const QPolygonF& pyg)
{
    if(pyg.size() != 2)
        return false;
    QPointF dp = pyg.back() - pyg.front();
    QRectF r(pyg.front(), pyg.back());
    if((dp.x() < margin())
            || (dp.y() < margin())
            || !scene()->sceneRect().contains(r))
        return false;
    return true;
}

void GraphicsRectItem::showRectFromCache()
{
    QPolygonF pyg = cache();
    if(pyg.size() != 2)
        return;
    QRectF r(pyg[0], pyg[1]);
    if(scene()->sceneRect().contains(r))
        setRect(r);
    update();
}
