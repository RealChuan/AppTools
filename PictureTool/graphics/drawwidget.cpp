#include "drawwidget.h"
#include "imageview.h"
#include "basicgraphicsitem.h"
#include "graphicsrectitem.h"
#include "graphicscircleitem.h"

#include <QtWidgets>
#include <QDebug>

class DrawWidgetPrivate{
public:
    DrawWidgetPrivate(QWidget *parent)
        : owner(parent){
        imageView = new ImageView(owner);
        shapeWidget = new QListWidget(owner);

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
    qDebug() << "onAddShape: " << type;
    switch (type) {
    case BasicGraphicsItem::RECT: shape = new GraphicsRectItem; break;
    case BasicGraphicsItem::CIRCLE: shape = new GraphicsCircleItem; break;
    case BasicGraphicsItem::POLYGON: break;
    case BasicGraphicsItem::RING: break;
    case BasicGraphicsItem::ROTATEDRECT: break;
    case BasicGraphicsItem::POINTS: break;
    case BasicGraphicsItem::LINE: break;
    case BasicGraphicsItem::ELLIPSE: break;
    case BasicGraphicsItem::TEXT: break;
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
    layout->addWidget(d->shapeWidget);
    layout->addWidget(d->imageView);

    QStringList shapeList;
    QMetaEnum shapeEnum = QMetaEnum::fromType<BasicGraphicsItem::Shape>();
    for(int i = 0; i < shapeEnum.keyCount(); i++)
        shapeList << shapeEnum.key(i);
    d->shapeWidget->addItems(shapeList);
    connect(d->shapeWidget, &QListWidget::itemClicked, this, &DrawWidget::onAddShape);
}
