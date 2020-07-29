#include "graphicsringitem.h"
#include "graphics.h"

#include <QGraphicsSceneHoverEvent>
#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QtMath>

QRectF Ring::boundingRect() const
{
    QPointF point = QPointF(maxRadius, maxRadius);
    return  QRectF(center - point, center + point);
}

QRectF Ring::minBoundingRect() const
{
    QPointF point = QPointF(minRadius, minRadius);
    return  QRectF(center - point, center + point);
}

bool Ring::isVaild() const
{
    return minRadius > 0 && maxRadius > minRadius;
}

struct GraphicsRingItemPrivate{
    Ring ring;
    GraphicsRingItem::MouseRegion mouseRegion = GraphicsRingItem::None;
};

GraphicsRingItem::GraphicsRingItem(QGraphicsItem *parent)
    : BasicGraphicsItem(parent)
    , d(new GraphicsRingItemPrivate)
{   
}

GraphicsRingItem::GraphicsRingItem(const Ring &ring, QGraphicsItem *parent)
    : BasicGraphicsItem(parent)
    , d(new GraphicsRingItemPrivate)
{
    setRing(ring);
}

GraphicsRingItem::~GraphicsRingItem()
{
}

void GraphicsRingItem::setRing(const Ring &ring)
{
    prepareGeometryChange();
    Ring ring_tmp = ring;
    if(!ring_tmp.isVaild())
        return;

    d->ring = ring;
    QPolygonF pts;
    computeCache(d->ring, pts);
    setCache(pts);
}

Ring GraphicsRingItem::ring() const
{
    return d->ring;
}

bool GraphicsRingItem::isValid() const
{
    return d->ring.isVaild();
}

int GraphicsRingItem::type() const
{
    return RING;
}

void GraphicsRingItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        QPointF pos = event->pos();
        setClickedPos(event->scenePos());
        if(!isValid() && scene()->sceneRect().contains(pos)){
            QPolygonF pts = cache();
            if(pts.size() < 4){
                pts.append(clickedPos());
                setCache(pts);
            }

            if(pts.size() == 3){
                QPointF center;
                double radius;
                if(!checkMaxCircleValid(pts, center, radius))
                    pts.removeLast();
                setCache(pts);
            }else if(pts.size() == 4){
                if(!checkMinCircleValid(pts))
                    pts.removeLast();
                setCache(pts);
                if(pts.size() == 4){
                    showRingFromCache();
                    return;
                }
            }
        }
    }else if(event->button() == Qt::RightButton && !isValid()){
        QPolygonF pts = cache();
        if(!pts.empty())
            pts.removeLast();
        setCache(pts);
    }

    update();
}

void GraphicsRingItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if((event->buttons() & Qt::LeftButton) == 0 || !isValid())
        return;

    if(!isSelected())
        setSelected(true);

    QPointF pos = event->scenePos();
    QPointF dp = pos - clickedPos();
    setClickedPos(pos);

    Ring ring = d->ring;
    switch (mouseRegion()) {
    case BasicGraphicsItem::DotRegion:
        switch (hoveredDotIndex()) {
        case 0: ring.maxRadius += dp.x(); break;
        case 1: ring.maxRadius += dp.y(); break;
        case 2: ring.maxRadius -= dp.x(); break;
        case 3: ring.maxRadius -= dp.y(); break;
        case 4: ring.minRadius += dp.x(); break;
        case 5: ring.minRadius += dp.y(); break;
        case 6: ring.minRadius -= dp.x(); break;
        case 7: ring.minRadius -= dp.y(); break;
        default: break;
        } break;
    case BasicGraphicsItem::All: ring.center += dp; break;
    case BasicGraphicsItem::None:
        switch (d->mouseRegion) {
        case InEdge0:
            setMyCursor(ring.center, pos);
            ring.minRadius = Graphics::distance(ring.center, pos);
            break;
        case InEdge1:
            setMyCursor(ring.center, pos);
            ring.maxRadius = Graphics::distance(ring.center, pos);
            break;
        default: break;
        } break;
    default: break;
    }

    QRectF rect = ring.boundingRect();
    if(scene()->sceneRect().contains(rect)
            && ring.isVaild()
            && ring.minRadius > margin()
            && (ring.maxRadius - ring.minRadius) > margin())
        setRing(ring);
}

void GraphicsRingItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    if(!isValid())
        return;

    BasicGraphicsItem::hoverMoveEvent(event);
    if(mouseRegion() == DotRegion)
        return;
    setMouseRegion(BasicGraphicsItem::None);

    QPointF pos = event->scenePos();
    if(abs(Graphics::distance(pos, d->ring.center) - d->ring.minRadius) < margin() / 3) {
        d->mouseRegion = InEdge0;
        setMyCursor(d->ring.center, pos);
    } else if(abs(Graphics::distance(pos, d->ring.center) - d->ring.maxRadius) < margin() / 3) {
        d->mouseRegion = InEdge1;
        setMyCursor(d->ring.center, pos);
    }else if(shape().contains(pos)){
        setMouseRegion(BasicGraphicsItem::All);
        setCursor(Qt::SizeAllCursor);
    }else
        unsetCursor();
}

void GraphicsRingItem::paint(QPainter *painter,
                             const QStyleOptionGraphicsItem *option,
                             QWidget*)
{
    painter->setRenderHint(QPainter::Antialiasing);
    double linew = 2 * pen().widthF() / painter->transform().m11();
    painter->setPen(QPen(LineColor, linew));
    setMargin(painter->transform().m11());

    QPolygonF pyg = cache();

    if(isValid()){
        painter->drawEllipse(d->ring.boundingRect());
        painter->drawEllipse(d->ring.minBoundingRect());
    }else{
        QPointF center;
        double radius;
        if(checkMaxCircleValid(pyg, center, radius)){
            painter->drawEllipse(QRectF(center.x() - radius, center.y() - radius,
                                        radius * 2, radius * 2));
        }
    }

    if(option->state & QStyle::State_Selected){
        drawAnchor(painter);
    }
}

void GraphicsRingItem::computeCache(const Ring& ring, QPolygonF& pts)
{
    pts.clear();
    for(int i = 0; i <= 270; i += 90){
        double x = ring.maxRadius * qCos( i * M_PI * 1.0 / 180.0) + ring.center.x();
        double y = ring.maxRadius * qSin( i * M_PI * 1.0 / 180.0) + ring.center.y();
        pts.append(QPointF(x, y));
    }

    for(int i = 45; i <= 315; i += 90){
        double x = ring.minRadius * qCos( i * M_PI * 1.0 / 180.0) + ring.center.x();
        double y = ring.minRadius * qSin( i * M_PI * 1.0 / 180.0) + ring.center.y();
        pts.append(QPointF(x, y));
    }
}

bool GraphicsRingItem::checkMaxCircleValid(const QPolygonF& pyg,
                                           QPointF& center,
                                           double& radius)
{
    if(pyg.size() < 3)
        return false;

    QPolygonF t = pyg.mid(0, 3);
    Graphics::calculateCircle(t, center, radius);
    if(radius < 1.5 * margin())
        return false;

    QRectF rect(center - QPointF(radius, radius),
                center + QPointF(radius, radius));
    return scene()->sceneRect().contains(rect);
}

bool GraphicsRingItem::checkMinCircleValid(const QPolygonF& pts)
{
    if(pts.size() < 4)
        return false;

    QPolygonF t = pts.mid(0, 3);
    QPointF center;
    double radius;
    Graphics::calculateCircle(t, center, radius);

    QLineF l(pts.back(), center);
    double min = l.length();
    return ((min > margin()) && (radius - min) > 0.5 * margin());
}

void GraphicsRingItem::showRingFromCache()
{
    QPolygonF pyg = cache();
    if(pyg.size() < 4)
        return;

    QPolygonF t = pyg.mid(0, 3);
    QPointF center;
    double radius;
    Graphics::calculateCircle(t, center, radius);
    QLineF l(pyg.back(), center);
    double min = l.length();

    setRing(Ring{center, min, radius});
}
