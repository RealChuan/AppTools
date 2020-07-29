#include "graphicsarcitem.h"
#include "graphics.h"

#include <QGraphicsSceneHoverEvent>
#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QtMath>
#include <QDebug>

#define MAX_PTS_SIZE 4

bool Arch::isValid() const
{
    return maxRadius > minRadius;
}

double Arch::diffLen() const
{
    return maxRadius - minRadius;
}

struct GraphicsArcItemPrivate{
    Arch arch;
    QPainterPath path;
    QPainterPath cachePath;
    QPainterPath bigPath;
    GraphicsArcItem::MouseRegion mouseRegion = GraphicsArcItem::None;
};

GraphicsArcItem::GraphicsArcItem(QGraphicsItem *parent)
    : BasicGraphicsItem(parent)
    , d(new GraphicsArcItemPrivate)
{
}

GraphicsArcItem::GraphicsArcItem(const Arch &arch, QGraphicsItem *parent)
    : BasicGraphicsItem(parent)
    , d(new GraphicsArcItemPrivate)
{
    setArc(arch);
    d->path = calculatePath(cache(), d->bigPath);
}

GraphicsArcItem::~GraphicsArcItem()
{
}

QPointF findAnotherPtOfLine(const QPointF basePt, const double r, const double angle)
{
    QLineF line;
    line.setP1(basePt);
    line.setLength(r);
    line.setAngle(angle);
    return line.p2();
}

QPolygonF calculateCache(const Arch &arch)
{
    QPointF pt00 = findAnotherPtOfLine(arch.center, arch.minRadius, arch.startAngle);
    QPointF pt01 = findAnotherPtOfLine(arch.center, arch.minRadius, arch.endAngle);
    QPointF pt10 = findAnotherPtOfLine(arch.center, arch.minRadius, (arch.startAngle + arch.endAngle) / 2.0);
    QPointF pt11 = findAnotherPtOfLine(arch.center, arch.maxRadius, (arch.startAngle + arch.endAngle) / 2.0);

    QPolygonF pts;
    pts.push_back(pt00);
    pts.push_back(pt01);
    pts.push_back(pt10);
    pts.push_back(pt11);

    return pts;
}

void GraphicsArcItem::setArc(const Arch &arch)
{
    prepareGeometryChange();
    Arch tmp_arch = d->arch;
    d->arch = arch;
    if(!isValid()){
        d->arch = tmp_arch;
        return;
    }

    QPolygonF pts = calculateCache(d->arch);
    setCache(pts);
}

Arch GraphicsArcItem::arch() const
{
    return d->arch;
}

bool GraphicsArcItem::isValid() const
{
    double minLen = margin() * qSqrt(2) / 2;
    return d->arch.isValid()
            && d->arch.minRadius >= minLen
            && d->arch.diffLen() >= minLen;
}

int GraphicsArcItem::type() const
{
    return ARC;
}

QRectF GraphicsArcItem::boundingRect() const
{
    if(!isValid())
        return BasicGraphicsItem::boundingRect();

    QPolygonF pts = d->path.toFillPolygon() + cache();
    QRectF rectF = pts.boundingRect();
    double addLen = margin() * qSqrt(2) / 2;
    rectF.adjust(-addLen, -addLen, addLen, addLen);
    return rectF;
}

QPainterPath GraphicsArcItem::shape() const
{
    if(!isValid() || cache().size() < MAX_PTS_SIZE)
        return BasicGraphicsItem::shape();

    return d->bigPath;
}

void GraphicsArcItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    setClickedPos(event->scenePos());
    QPolygonF pts = cache();
    if(pts.size() < MAX_PTS_SIZE){
        pts.append(clickedPos());
        pointsChanged(pts);
        update();
    }
}

QPointF lineSetLength(const QPointF p1,
                      const QPointF p2,
                      const double len)
{
    QLineF line(p1, p2);
    line.setLength(len);
    return line.p2();
}

void GraphicsArcItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(!isValid())
        return;

    double dis = Graphics::distance(d->arch.center, event->scenePos());
    QPointF deltaPt = event->scenePos() - clickedPos();
    QPolygonF pts_tmp = cache();

    switch (mouseRegion()) {
    case BasicGraphicsItem::All: pts_tmp.translate(deltaPt);break;
    case BasicGraphicsItem::None: {
        switch (d->mouseRegion) {
        case InEdge0: {
            setMyCursor(d->arch.center, event->scenePos());
            for (int i = 0; i < 3; ++i)
                pts_tmp[i] = lineSetLength(d->arch.center, pts_tmp[i], dis);
            break;
        }
        case InEdge1:
            setMyCursor(d->arch.center, event->scenePos());
            pts_tmp[3] = lineSetLength(d->arch.center, pts_tmp[3], dis);
            break;
        case InEdgeH:
        case InEdgeL:{
            Arch arch = d->arch;
            if(d->mouseRegion == InEdgeL){
                arch.startAngle = QLineF(arch.center, event->scenePos()).angle();
                setCursor(Graphics::curorFromAngle(arch.startAngle));
            }else{
                arch.endAngle = QLineF(arch.center, event->scenePos()).angle();
                setCursor(Graphics::curorFromAngle(arch.endAngle));
            }
            while(arch.startAngle > arch.endAngle)
                arch.endAngle += 360;
            if(arch.endAngle - arch.startAngle > 360)
                arch.endAngle -= 360;
            pts_tmp = calculateCache(arch);
            break;
        }
        default: break;
        }break;
    }
    case BasicGraphicsItem::DotRegion:{
        int index = hoveredDotIndex();
        if(index != -1)
            pts_tmp[index] += deltaPt;
        break;
    }
    default: break;
    }

    pointsChanged(pts_tmp);
    setClickedPos(event->scenePos());
    update();
}

void GraphicsArcItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    QPolygonF m_pts = cache();
    QPointF pos = event->scenePos();

    if(m_pts.size() < MAX_PTS_SIZE && m_pts.size() >= 2){
        m_pts.push_back(pos);
        calculateCurrentPath(m_pts);
    }

    if(!isValid())
        return;

    d->mouseRegion = None;
    BasicGraphicsItem::hoverMoveEvent(event);
    if(mouseRegion() == BasicGraphicsItem::DotRegion)
        return;
    setMouseRegion(BasicGraphicsItem::None);

    QPointF p1 = findAnotherPtOfLine(d->arch.center, d->arch.maxRadius, d->arch.startAngle);
    QPointF p2 = findAnotherPtOfLine(d->arch.center, d->arch.maxRadius, d->arch.endAngle);

    QLineF line1(p1, cache()[0]);
    QLineF line2(p2, cache()[1]);

    if(qAbs(Graphics::distance(pos, d->arch.center) - d->arch.minRadius) < margin() / 3) {
        d->mouseRegion = InEdge0;
        setMyCursor(d->arch.center, pos);
    } else if(qAbs(Graphics::distance(pos, d->arch.center) - d->arch.maxRadius) < margin() / 3) {
        d->mouseRegion = InEdge1;
        setMyCursor(d->arch.center, pos);
    } else if(Graphics::boundingFromLine(line1, margin() / 4)
              .containsPoint(pos, Qt::OddEvenFill)){
        d->mouseRegion = InEdgeL;
        setCursor(Graphics::curorFromAngle(line1.angle()));
    }else if(Graphics::boundingFromLine(line2, margin() / 4)
             .containsPoint(pos, Qt::OddEvenFill)){
        d->mouseRegion = InEdgeH;
        setCursor(Graphics::curorFromAngle(line2.angle()));
    }else if(d->path.contains(pos)){
        setMouseRegion(BasicGraphicsItem::All);
        setCursor(Qt::SizeAllCursor);
    }else
        unsetCursor();
}

void GraphicsArcItem::paint(QPainter *painter,
                            const QStyleOptionGraphicsItem *option,
                            QWidget*)
{
    painter->setRenderHint(QPainter::Antialiasing);
    double linew = 2 * pen().widthF() / painter->transform().m11();
    painter->setPen(QPen(LineColor, linew));
    setMargin(painter->transform().m11());

    if(d->path.isEmpty())
        painter->drawPath(d->cachePath);
    else
        painter->drawPath(d->path);
    if(option->state & QStyle::State_Selected)
        drawAnchor(painter);
}

bool inTop(const QPointF linePt0, const QPointF linePt1, const QPointF pt)
{
    if(linePt0.x() - linePt1.x() == 0.0){
        if(pt.x() > linePt0.x())
            return false;
        return true;
    }

    bool isInTop = true;
    double k = 0;
    double b = 0;
    k = (linePt0.y() - linePt1.y()) / (linePt0.x() - linePt1.x());
    b = linePt0.y() - k * linePt0.x();
    if((pt.x() * k + b - pt.y()) < 0)
        isInTop = false;
    return isInTop;
}

void calculateArc(QPointF center,
                  double radius,
                  QPointF p1,
                  QPointF p2,
                  bool isInTop,
                  QPainterPath &path,
                  double &startAngle,
                  double &endAngle,
                  double &deltaAngle)
{
    // 先确定起点和终点 当两个点X相等时，起始点无所谓是哪个
    QPointF startPoint ;
    QPointF endPoint ;
    if(p1.x() > p2.x()) {
        startPoint = p1;
        endPoint = p2;
    } else {
        startPoint = p2;
        endPoint = p1;
    }

    // 计算角度，保证终点角度大于起点角度
    startAngle = QLineF(center, startPoint).angle();
    endAngle = QLineF(center, endPoint).angle();

    if(startAngle > endAngle)
        endAngle += 360;

    deltaAngle = isInTop ? (endAngle - startAngle)
                         : (endAngle - startAngle - 360);

    QRectF rect(QPointF(center.x() - radius, center.y() - radius),
                QPointF(center.x() + radius, center.y() + radius));
    QPainterPath pathTemp(startPoint);
    path = pathTemp;
    // deltaAngle 为 负数时，顺时针
    path.arcTo(rect, startAngle, deltaAngle);
}

void GraphicsArcItem::pointsChanged(const QPolygonF& pts)
{
    if(pts.size() > 2){
        double deltaAngle = QLineF(pts[0], pts[1])
                .angleTo(QLineF(pts[0], pts[2]));
        if(deltaAngle < 0.00001 || deltaAngle > 355.99999)
            return;
    }

    if(!calculateCurrentPath(pts))
        return;

    setCache(pts);

    if(pts.size() == MAX_PTS_SIZE)
        calculateArch(pts);
}

void GraphicsArcItem::calculateArch(const QPolygonF& pts)
{
    double startAngle_ = 0;
    double endAngle_ = 0;

    QPointF center(0, 0);
    double radius0 = 0;
    double radius1 = 0;
    double startAngle = 0;
    double endAngle = 0;
    double deltaAngle = 0;

    QPolygonF pts_tmp = pts;
    QPointF endPt = pts_tmp[pts_tmp.size() - 1];
    pts_tmp.pop_back();
    // 计算方向得出旋转角度
    bool isInTop = inTop(pts[0], pts[1], endPt);
    Graphics::calculateCircle(pts_tmp, center, radius0);
    QPainterPath path1;
    calculateArc(center, radius0, pts[0], pts[1], isInTop, path1, startAngle, endAngle, deltaAngle);
    radius1 = Graphics::distance(endPt, center);

    endAngle = startAngle + deltaAngle;
    if(deltaAngle > 0) {
        startAngle_ = startAngle;
        endAngle_ = endAngle;
    } else {
        startAngle_ = endAngle + 360;
        endAngle_ = startAngle + 360;
    }

    Arch arch{center, qMin(radius0, radius1), qMax(radius0, radius1), startAngle_, endAngle_};
    setArc(arch);
}

bool GraphicsArcItem::calculateCurrentPath(const QPolygonF &pts)
{
    if(pts.size() < MAX_PTS_SIZE - 1)
        return true;

    QPainterPath shape;
    QPainterPath path = calculatePath(pts, shape);

    if(path.isEmpty())
        return false;

    if(ptsAndPathVaild(path, pts)){
        d->cachePath.clear();
        d->path = path;
        d->bigPath = shape;
        return true;
    } else if(cache().size() != MAX_PTS_SIZE){
        d->path.clear();
        d->cachePath = path;
    }

    return false;
}

QPainterPath GraphicsArcItem::calculatePath(const QPolygonF &pts, QPainterPath &shape)
{
    QPainterPath path_temp;
    QPointF center(0, 0);
    double radius0 = 0;
    double radius1 = 0;
    double startAngle = 0;
    double endAngle = 0;
    double deltaAngle = 0;

    QPolygonF pts_tmp = pts;
    QPointF endPt = pts_tmp[pts_tmp.size() - 1];
    if(pts.size() == MAX_PTS_SIZE)
        pts_tmp.pop_back();
    // 计算方向得出旋转角度
    bool isInTop = inTop(pts[0], pts[1], endPt);
    Graphics::calculateCircle(pts_tmp, center, radius0);
    calculateArc(center, radius0, pts[0], pts[1], isInTop,
            path_temp, startAngle, endAngle, deltaAngle);

    if(deltaAngle <= 0){
        if(endAngle < 0){
            endAngle += 360;
            startAngle += 360;
        }
        qSwap(startAngle, endAngle);
        deltaAngle = -deltaAngle;
    }

    if(pts.size() == MAX_PTS_SIZE - 1)
        return path_temp;

    radius1 = Graphics::distance(endPt, center);

    double radiusIn = qMin(radius0, radius1);
    double radiusOut = qMax(radius0, radius1);
    double num = margin() * qSqrt(2) / 2;

    if((radiusIn < num) || ((radiusOut - radiusIn) < num))
        return QPainterPath();

    QRectF rect0(center.x() - radiusIn, center.y() - radiusIn, radiusIn*2, radiusIn*2);
    QRectF rect1(center.x() - radiusOut, center.y() - radiusOut, radiusOut*2, radiusOut*2);

    QPainterPath path_real(center);
    path_real.arcTo(rect1, startAngle, deltaAngle);

    QPainterPath subPath1;
    subPath1.addEllipse(rect0);
    path_real -= subPath1;

    //big path
    radiusIn -= num;
    radiusOut += num;

    QRectF rect00(center.x() - radiusIn, center.y() - radiusIn, radiusIn*2, radiusIn*2);
    QRectF rect11(center.x() - radiusOut, center.y() - radiusOut, radiusOut*2, radiusOut*2);

    double addAngle = qAsin(num/(radiusIn + num)) * 180 / M_PI;
    startAngle -= addAngle;
    deltaAngle += 2 * addAngle;

    shape.clear();
    shape.setFillRule(Qt::WindingFill);
    shape.moveTo(center);
    shape.arcTo(rect11, startAngle, deltaAngle);

    QPainterPath subPath;
    subPath.addEllipse(rect00);
    shape -= subPath;

    return path_real;
}

bool GraphicsArcItem::ptsAndPathVaild(const QPainterPath& path, const QPolygonF& pts)
{
    QPolygonF polygon = path.toFillPolygon() + pts;
    QRectF rect = polygon.boundingRect();
    QRectF scenceRect = scene()->sceneRect();
    if(scenceRect.contains(rect))
        return true;
    return false;
}
