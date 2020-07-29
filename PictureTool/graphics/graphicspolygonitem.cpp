#include "graphicspolygonitem.h"
#include "graphics.h"

#include <QGraphicsSceneHoverEvent>
#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

struct GraphicsPolygonItemPrivate{
    QPolygonF polygon;
};

GraphicsPolygonItem::GraphicsPolygonItem(QGraphicsItem *parent)
    : BasicGraphicsItem(parent)
    , d(new GraphicsPolygonItemPrivate)
{
}

GraphicsPolygonItem::GraphicsPolygonItem(const QPolygonF& polygon,
                                         QGraphicsItem* parent)
    : BasicGraphicsItem(parent)
    , d(new GraphicsPolygonItemPrivate)
{
    setPolygon(polygon);
}

GraphicsPolygonItem::~GraphicsPolygonItem()
{
}

void GraphicsPolygonItem::setPolygon(const QPolygonF &pyg)
{
    prepareGeometryChange();
    QPolygonF pyg_tmp = d->polygon;
    d->polygon = pyg;
    if(!isValid()){
        d->polygon = pyg_tmp;
        return;
    }
    setCache(pyg);
}

QPolygonF GraphicsPolygonItem::polygon() const
{
    return d->polygon;
}

bool GraphicsPolygonItem::isValid() const
{
    QRectF rectF = d->polygon.boundingRect();
    return (d->polygon.size() >= 3)
            && rectF.width() >= margin()
            && rectF.height() >= margin();
}

int GraphicsPolygonItem::type() const
{
    return POLYGON;
}

void GraphicsPolygonItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        QPointF pos = event->pos();
        setClickedPos(event->scenePos());

        QPolygonF pyg = cache();
        if(!isValid() && scene()->sceneRect().contains(pos)){
            pyg.append(clickedPos());
            if(pyg.size() >= 3){
                double l = Graphics::distance(pyg.front(), pyg.back());
                if(l <= margin() / 2){
                    pyg.removeLast();
                    setCache(pyg);
                    showPolygonFromCache();
                }
            }
            setCache(pyg);
        }
    }else if(event->button() == Qt::RightButton && !isValid()){
        QPolygonF pyg_tmp = cache();
        if(!pyg_tmp.isEmpty())
            pyg_tmp.removeLast();
        setCache(pyg_tmp);
    }

    update();
}

void GraphicsPolygonItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if((event->buttons() & Qt::LeftButton) == 0 || !isValid())
        return;

    if(!isSelected())
        setSelected(true);

    QPointF p = event->scenePos();
    QPolygonF pyg = cache();

    switch (mouseRegion()) {
    case DotRegion: pyg.replace(hoveredDotIndex(), p); break;
    case All: {
        QPointF dp = p - clickedPos();
        setClickedPos(p);
        pyg.translate(dp);
    } break;
    default: break;
    }

    if(checkCacheValid(pyg))
        setCache(pyg);

    showPolygonFromCache();
}

void GraphicsPolygonItem::paint(QPainter *painter,
                                const QStyleOptionGraphicsItem *option,
                                QWidget*)
{
    painter->setRenderHint(QPainter::Antialiasing);
    double linew = 2 * pen().widthF() / painter->transform().m11();
    painter->setPen(QPen(LineColor, linew));
    setMargin(painter->transform().m11());

    if(isValid())
        painter->drawPolygon(d->polygon);
    else
        painter->drawPolyline(cache());

    if(option->state & QStyle::State_Selected)
        drawAnchor(painter);
}

bool GraphicsPolygonItem::checkCacheValid(const QPolygonF &pyg)
{
    if(pyg.size() < 3)
        return false;

    QRectF rectF = pyg.boundingRect();
    return scene()->sceneRect().contains(rectF)
            && rectF.width() >= margin()
            && rectF.height() >= margin();
}

void GraphicsPolygonItem::showPolygonFromCache()
{
    QPolygonF pyg = cache();
    if(checkCacheValid(pyg))
        setPolygon(pyg.toPolygon());
}

