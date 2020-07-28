#include "graphicscircleitem.h"
#include "graphics.h"

#include <QGraphicsSceneHoverEvent>
#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QtMath>
#include <QDebug>

#define MIN_RADIUS 9

Circle::Circle()
    : center()
    , radius()
{
}

Circle::Circle(const QPointF &center, const double radius)
    : center(center)
    , radius(radius)
{
}

bool Circle::isVaild()
{
    return qAbs(radius) > MIN_RADIUS;
}

QRectF Circle::boundingRect()
{
    return QRectF(center - QPointF(radius, radius),
                  center + QPointF(radius, radius));
}



struct GraphicsCircleItemPrivate{
    Circle circle;
};

GraphicsCircleItem::GraphicsCircleItem(QGraphicsItem *parent)
    : BasicGraphicsItem(parent)
    , d(new GraphicsCircleItemPrivate)
{

}

GraphicsCircleItem::GraphicsCircleItem(const Circle &cicrle,
                                       QGraphicsItem *parent)
    : BasicGraphicsItem(parent)
    , d(new GraphicsCircleItemPrivate)
{
    setCircle(cicrle);
}

GraphicsCircleItem::~GraphicsCircleItem()
{
}

void GraphicsCircleItem::setCircle(const Circle &circle)
{
    prepareGeometryChange();
    Circle circle_tmp = circle;
    if(!circle_tmp.isVaild())
        return;
    d->circle = circle;

    QPolygonF pts;
    computeCache(d->circle, pts);
    setCache(pts);
}

Circle GraphicsCircleItem::circle() const
{
    return d->circle;
}

bool GraphicsCircleItem::isValid() const
{
    return d->circle.isVaild();
}

int GraphicsCircleItem::type() const
{
    return CIRCLE;
}

void GraphicsCircleItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        QPointF pos = event->pos();
        setClickedPos(event->scenePos());

        if(!isValid() && scene()->sceneRect().contains(pos)){
            QPolygonF pts_tmp = this->cache();

            if(pts_tmp.size() < 3)
                pts_tmp.append(clickedPos());

            if(pts_tmp.size() == 3){
                Circle circle;
                if(checkCacheValid(circle, pts_tmp)){
                    setCache(pts_tmp);
                    showCircleFromCache(circle);
                    return;
                }else
                    pts_tmp.removeLast();
            }
            setCache(pts_tmp);
        }
    }else if(event->button() == Qt::RightButton && !isValid()){
        QPolygonF pts_tmp = cache();
        if(!pts_tmp.empty())
            pts_tmp.removeLast();
        setCache(pts_tmp);
    }

    update();
}

void GraphicsCircleItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if((event->buttons() & Qt::LeftButton) == 0
            || !isValid())
        return;

    if(!isSelected())
        setSelected(true);

    QPointF pos = event->scenePos();
    QPointF dp = pos - clickedPos();
    setClickedPos(pos);

    double radius = d->circle.radius;
    QPointF center = d->circle.center;
    switch (mouseRegion()) {
    case DotRegion:
        switch (hoveredDotIndex()) {
        case 0: radius += dp.x(); break;
        case 1: radius += dp.y(); break;
        case 2: radius -= dp.x(); break;
        case 3: radius -= dp.y(); break;
        default: break;
        }
        break;
    case Edge:{
        QLineF lineF = QLineF(d->circle.center, pos);
        setCursor(Graphics::curorFromAngle(
                      Graphics::ConvertTo360(lineF.angle() - 90)));
        radius = lineF.length();
    } break;
    case All: center += dp; break;
    default: break;
    }

    QRectF rect(center - QPointF(radius, radius),
                center + QPointF(radius, radius));

    if(scene()->sceneRect().contains(rect) && radius >= MIN_RADIUS)
        setCircle(Circle(center, radius));
}

void GraphicsCircleItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    if(!isValid())
        return;

    BasicGraphicsItem::hoverMoveEvent(event);
    if(mouseRegion() == DotRegion)
        return;

    QPointF pos = event->scenePos();
    if(qAbs(Graphics::distance(pos, d->circle.center) - d->circle.radius)
            < margin() / 3){
        setMouseRegion(Edge);
        double angle = QLineF(d->circle.center, pos).angle();
        setCursor(Graphics::curorFromAngle(Graphics::ConvertTo360(angle - 90)));
    }
}

void GraphicsCircleItem::paint(QPainter *painter,
                               const QStyleOptionGraphicsItem *option,
                               QWidget*)
{
    painter->setRenderHint(QPainter::Antialiasing, true);
    double linew =  pen().widthF() / painter->transform().m11();
    painter->setPen(QPen(pen().color(), linew));
    setMargin(painter->transform().m11());

    if(isValid())
        painter->drawEllipse(QRectF(d->circle.center.x() - d->circle.radius,
                                    d->circle.center.y() - d->circle.radius,
                                    d->circle.radius * 2,
                                    d->circle.radius * 2));

    if((option->state & QStyle::State_Selected) && cache().count() > 0)
        drawAnchor(painter);
}

void GraphicsCircleItem::computeCache(const Circle &circle, QPolygonF &cache)
{
    cache.clear();
    for(int i = 0; i <= 270; i += 90){
        double x = circle.radius * qCos( i * M_PI * 1.0 / 180.0)
                + circle.center.x();
        double y = circle.radius * qSin( i * M_PI * 1.0 / 180.0)
                + circle.center.y();
        cache.append(QPointF(x, y));
    }
}

bool GraphicsCircleItem::checkCacheValid(Circle &circle, const QPolygonF &pyg)
{
    if(pyg.size() < 3)
        return false;

    Graphics::calculateCircle(pyg, circle.center, circle.radius);

    if(circle.isVaild())
        return scene()->sceneRect().contains(circle.boundingRect());

    return false;
}

void GraphicsCircleItem::showCircleFromCache(const Circle &circle)
{
    setCircle(circle);
    update();
}
