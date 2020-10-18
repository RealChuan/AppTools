#include "drawwidget.h"

#include <graphics/imageview.h>
#include <graphics/basicgraphicsitem.h>
#include <graphics/graphicsrectitem.h>
#include <graphics/graphicscircleitem.h>
#include <graphics/graphicspolygonitem.h>
#include <graphics/graphicsringitem.h>
#include <graphics/graphicsarcitem.h>
#include <graphics/graphicsrotatedrectitem.h>
#include <graphics/graphicslineitem.h>

#include <QtWidgets>
#include <QDebug>

using namespace Graphics;

class DrawWidgetPrivate{
public:
    DrawWidgetPrivate(QWidget *parent)
        : owner(parent){
        imageView = new ImageView(owner);
        shapeWidget = new QListWidget(owner);
        shapeWidget->setObjectName("ShapeWidget");

        QPixmap pixmap(4096, 2160);
        pixmap.fill(Qt::white);
        imageView->setPixmap(pixmap);
    }
    QWidget *owner;
    ImageView *imageView;
    QListWidget *shapeWidget;
    QVector<BasicGraphicsItem*> graphicsItems;
};

DrawWidget::DrawWidget(QWidget *parent)
    : QWidget(parent)
    , d(new DrawWidgetPrivate(this))
{
    setupUI();
}

DrawWidget::~DrawWidget()
{
    if(!d->graphicsItems.isEmpty()){
        qDeleteAll(d->graphicsItems);
        d->graphicsItems.clear();
    }
}

void DrawWidget::onAddShape(QListWidgetItem *item)
{
    if(!d->graphicsItems.isEmpty() && !d->graphicsItems.last()->isValid())
        delete d->graphicsItems.takeLast();

    BasicGraphicsItem *shape = nullptr;
    int type = d->shapeWidget->row(item) + 1;
    switch (type) {
    case BasicGraphicsItem::LINE: shape = new GraphicsLineItem; break;
    case BasicGraphicsItem::RECT: shape = new GraphicsRectItem; break;
    case BasicGraphicsItem::CIRCLE: shape = new GraphicsCircleItem; break;
    case BasicGraphicsItem::POLYGON: shape = new GraphicsPolygonItem; break;
    case BasicGraphicsItem::RING: shape = new GraphicsRingItem; break;
    case BasicGraphicsItem::ARC: shape = new GraphicsArcItem; break;
    case BasicGraphicsItem::ROTATEDRECT: shape = new GraphicsRotatedRectItem; break;
    default: break;
    }

    if(shape){
        d->graphicsItems.append(shape);
        d->imageView->scene()->addItem(shape);
        connect(shape, &BasicGraphicsItem::deleteMyself,
                this, &DrawWidget::onRemoveShape, Qt::UniqueConnection);
    }
}

void DrawWidget::onRemoveShape()
{
    BasicGraphicsItem *item = qobject_cast<BasicGraphicsItem*>(sender());
    if(!item)
        return;
    d->graphicsItems.removeOne(item);
    item->deleteLater();
}

void DrawWidget::setupUI()
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(d->shapeWidget);
    layout->addWidget(d->imageView);

    QStringList shapeList;
    QMetaEnum shapeEnum = QMetaEnum::fromType<BasicGraphicsItem::Shape>();
    for(int i = 0; i < shapeEnum.keyCount(); i++)
        shapeList << shapeEnum.key(i);
    d->shapeWidget->addItems(shapeList);
    connect(d->shapeWidget, &QListWidget::itemClicked, this, &DrawWidget::onAddShape);
}
