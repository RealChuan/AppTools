#include "graphicsrotatedrectitem.h"
#include "graphics.h"

#include <QGraphicsSceneHoverEvent>
#include <QGraphicsScene>
#include <QtMath>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

bool RotatedRect::isValid() const
{
    return width > 0 && height > 0;
}

struct GraphicsRotatedRectItemPrivate{
    RotatedRect rotatedRect;
    bool rotatedHovered = false;
    bool linehovered = false;
    QLineF hoveredLine;
};

GraphicsRotatedRectItem::GraphicsRotatedRectItem(QGraphicsItem *parent)
    : BasicGraphicsItem(parent)
    , d(new GraphicsRotatedRectItemPrivate)
{
    setAcceptHoverEvents(true);
}

GraphicsRotatedRectItem::GraphicsRotatedRectItem(const RotatedRect &rotatedRect,
                                                 QGraphicsItem *parent)
    : BasicGraphicsItem(parent)
    , d(new GraphicsRotatedRectItemPrivate)
{
    setAcceptHoverEvents(true);
    setRotatedRect(rotatedRect);
}

GraphicsRotatedRectItem::~GraphicsRotatedRectItem()
{
}

void GraphicsRotatedRectItem::setRotatedRect(const RotatedRect &rotatedRect)
{
    prepareGeometryChange();
    RotatedRect rotatedRect_tmp = rotatedRect;
    if(!rotatedRect_tmp.isValid())
        return;
    d->rotatedRect = rotatedRect;
    QPolygonF pts = cacheFromRotated(rotatedRect);
    setCache(pts);
}

RotatedRect GraphicsRotatedRectItem::rotatedRect() const
{
    return d->rotatedRect;
}

bool GraphicsRotatedRectItem::isValid() const
{
    return d->rotatedRect.isValid();
}

int GraphicsRotatedRectItem::type() const
{
    return ROTATEDRECT;
}

void GraphicsRotatedRectItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        QPointF p = event->pos();
        this->setClickedPos(event->scenePos());
        if(!isValid() && scene() && scene()->sceneRect().contains(p)){
            QPolygonF cache = this->cache();
            cache << event->scenePos();
            if(cache.count() < 3)
                setCache(cache);

            if(cache.count() == 3 && !checkCacheValid(cache))
                cache.removeLast();

            if(checkCacheValid(cache)){
                setCache(cache);
                showRotatedFromCache();
                return;
            }
        }
    }else if(event->button() == Qt::RightButton && !isValid()){
        QPolygonF cache = this->cache();
        if(!cache.empty())
            cache.removeLast();
        setCache(cache);
    }

    update();
    BasicGraphicsItem::mousePressEvent(event);
}

void GraphicsRotatedRectItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if((event->buttons() & Qt::LeftButton) == 0 || !isValid())
        return;

    setSelected(true);
    QPointF p = event->scenePos();
    QPointF dp = p - clickedPos();

    RotatedRect rrt = d->rotatedRect;
    QPointF c = rrt.center;
    QPolygonF pts = cache();
    QLineF l0(c, p);
    QLineF l1(c, clickedPos());
    double m = qSqrt(dp.x() * dp.x() + dp.y() * dp.y());
    if(d->rotatedHovered){
        double angle = l0.angleTo(l1);
        rrt.angle += angle;
        rrt.angle = Graphics::ConvertTo360(rrt.angle);
        setCursor(Graphics::curorFromAngle(360 - rrt.angle));
    }else if(d->linehovered){
        QLineF dl = d->hoveredLine.normalVector();
        QPointF p1 = d->hoveredLine.p1();
        QPointF p2 = d->hoveredLine.p2();

        int index0 = pts.indexOf(p1);
        int index1 = pts.indexOf(p2);
        if(index0 < 0 || index1 < 0)
            return;

        bool strech = l0.length() > l1.length();
        QPointF p3 = p1 + QPointF(dl.dx() * m, dl.dy() * m)
                /dl.length() * (strech ? 1 : -1);
        QPointF p4 = p2 + QPointF(dl.dx() * m, dl.dy() * m)
                /dl.length() * (strech ? 1 : -1);

        pts.replace(index0, p3);
        pts.replace(index1, p4);
        d->hoveredLine = QLineF(p3, p4);

        rrt.width = QLineF(pts.at(0), pts.at(1)).length();
        rrt.height = QLineF(pts.at(0), pts.at(3)).length();
        rrt.center = (pts.at(0) + pts.at(2)) / 2;

    }else{
        if(this->mouseRegion() == DotRegion){

        }else if(this->mouseRegion() == All){
            rrt.center += dp;
        }
    }

    pts = cacheFromRotated(rrt);
    if(checkCacheValid(pts)){
        setRotatedRect(rrt);

    }

    setClickedPos(p);
}

void GraphicsRotatedRectItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    if(!isValid())
        return;

    QPointF p = event->scenePos();
    QPointF c = d->rotatedRect.center;

    QPolygonF ply = this->cache();
    QPointF c1 = (ply.at(1) + ply.at(2)) / 2;
    QLineF l = QLineF(c, c1);
    l = QLineF(c, l.pointAt(0.9));

    ply = Graphics::boundingFromLine(l, margin() / 4);
    if(ply.containsPoint(p, Qt::OddEvenFill)){
        d->rotatedHovered = true;
        setCursor(Graphics::curorFromAngle(l.angle()));
        return;
    }

    ply = this->cache();
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

    d->rotatedHovered = false;
    d->linehovered = false;
    BasicGraphicsItem::hoverMoveEvent(event);
}

void GraphicsRotatedRectItem::paint(QPainter *painter,
                                    const QStyleOptionGraphicsItem *option,
                                    QWidget*)
{
    painter->setRenderHint(QPainter::Antialiasing);
    double linew = 2 * pen().widthF() / painter->transform().m11();
    painter->setPen(QPen(LineColor, linew));
    setMargin(painter->transform().m11());

    if(isValid()){
        QPointF c = d->rotatedRect.center;
        painter->drawPolygon(this->cache());
        if(option->state & QStyle::State_Selected){
            painter->setPen(QPen(pen().color().darker(), linew));
            QPointF c1 = (cache().at(1) + cache().at(2)) / 2;
            QLineF l = QLineF(c, c1);
            l = QLineF(c, l.pointAt(0.9));
            painter->drawLine(l);
            painter->drawEllipse(c, 5, 5);
        }
    }

    if(!isValid() || (option->state & QStyle::State_Selected)){
        drawAnchor(painter);
    }
}

QPointF rotate(const QPointF& p, double angle)
{
    double _angle = Graphics::ConvertTo360(angle) * M_PI * 2 / 360.0;
    double cos = std::cos(_angle);
    double sin = std::sin(_angle);
    double _x = cos * p.x() - sin * p.y();
    double _y = sin * p.x() + cos * p.y();
    return QPointF(_x, _y);
}

QPolygonF GraphicsRotatedRectItem::cacheFromRotated(const RotatedRect &rotated)
{
    QPointF p0 = QPointF(-rotated.width / 2, -rotated.height / 2);
    QPointF p1 = QPointF(rotated.width / 2,  -rotated.height / 2);
    QPointF p2 = QPointF(rotated.width / 2,   rotated.height / 2);
    QPointF p3 = QPointF(-rotated.width / 2,  rotated.height / 2);

    QPolygonF pts;
    pts << rotated.center + rotate(p0, rotated.angle);
    pts << rotated.center + rotate(p1, rotated.angle);
    pts << rotated.center + rotate(p2, rotated.angle);
    pts << rotated.center + rotate(p3, rotated.angle);

    return pts;
}

bool GraphicsRotatedRectItem::checkCacheValid(const QPolygonF& pts)
{
    double w, h;
    if(pts.count() < 3)
        return false;
    else if(pts.count() == 3){
        QLineF l = QLineF(pts.at(0), pts.at(1));
        QLineF l1 = l.normalVector();
        QPointF p;
        l.intersects(l1, &p);
        w = l.length();
        h = QLineF(p, pts.at(2)).length();
    }else{
        w = QLineF(pts.at(0), pts.at(1)).length();
        h = QLineF(pts.at(0), pts.at(3)).length();
    }

    QRectF r = pts.boundingRect();
    if(w < margin()
            || h < margin()
            || !scene()->sceneRect().contains(r))
        return false;
    return true;
}

void GraphicsRotatedRectItem::showRotatedFromCache()
{
    // compute rotatedRect
    QPolygonF ply = this->cache();
    if(ply.count() < 3)
        return;

    RotatedRect rrt;
    rrt.width = QLineF(ply.at(0), ply.at(1)).length();
    rrt.height = QLineF(ply.at(1), ply.at(2)).length();
    rrt.center = (ply.at(0) + ply.at(2)) / 2;
    rrt.angle = Graphics::ConvertTo360(QLineF(ply.at(0), ply.at(1)).angle());

    setRotatedRect(rrt);
}
