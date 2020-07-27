#ifndef BASICGRAPHICSITEM_H
#define BASICGRAPHICSITEM_H

#include <QGraphicsPixmapItem>
#include <QObject>

#include "graphics_global.h"

struct BasicGraphicsItemPrivate;
class GRAPHICS_EXPORT BasicGraphicsItem : public QObject, public QAbstractGraphicsShapeItem
{
    Q_OBJECT
public:
    enum Shape{
        RECT = 1,
        CIRCLE,
        POLYGON,
        RING,
        ARC,
        ROTATEDRECT,
        POINTS,
        LINE,
        ELLIPSE,
        TEXT,
        EXTEND = 1024
    }Q_ENUMS(Shape);

    enum MouseRegion{
        DotRegion,
        All,
        None
    };

    BasicGraphicsItem(QGraphicsItem *parent = nullptr);
    ~BasicGraphicsItem() override;

    virtual bool itemValid() const  = 0;
    virtual int type() const override = 0;
    virtual QRectF boundingRect() const override;

    void setName(const QString &name);
    QString name() const;

    void setMargin(double m11);
    double margin() const;

    void setItemEditable(bool editable);

signals:
    void itemChanged();

protected:
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    void setCache(const QPolygonF&);
    QPolygonF cache() const;

    void setClickedPos(const QPointF& p);
    QPointF clickedPos() const;

    MouseRegion mouseRegion() const;
    void setMouseRegion(const MouseRegion);

    int hoveredDotIndex() const;

    void drawAnchor(QPainter* painter);

private:
    QScopedPointer<BasicGraphicsItemPrivate> d;
};

#endif // BASICGRAPHICSITEM_H
