#include "commonwidget.h"

#include <QtWidgets>

namespace Control {

class CommonWidgetPrivate{
public:
    CommonWidgetPrivate(QWidget *parent)
        : owner(parent){
        iconLabel = new QLabel(owner);
        iconLabel->setObjectName("iconLabel");
        titleLabel = new QLabel(qAppName(), owner);
        titleLabel->setObjectName("titleLabel");
        minButton = new QToolButton(owner);
        minButton->setObjectName("minButton");
        minButton->setToolTip(QObject::tr("Minimize"));
        maxButton = new QToolButton(owner);
        maxButton->setObjectName("maxButton");
        maxButton->setToolTip(QObject::tr("Maximize"));
        restoreButton = new QToolButton(owner);
        restoreButton->setObjectName("restoreButton");
        restoreButton->setToolTip(QObject::tr("Restore"));
        centralWidget = new QWidget(owner);
        centralWidget->setObjectName("CommonCentralWidget");
        titleBar = new QWidget(owner);
        layout = new QHBoxLayout(owner);
        layout->setSpacing(0);
        layout->setContentsMargins(shadowPadding, shadowPadding, shadowPadding, shadowPadding);
    }
    QWidget *owner;
    QLabel *iconLabel;
    QLabel *titleLabel;
    QToolButton *minButton;
    QToolButton *maxButton;
    QToolButton *restoreButton;
    QWidget *titleWidget;
    QWidget *centralWidget;
    QWidget *titleBar;
    QHBoxLayout *layout;

    int shadowPadding = 10;

    //记录鼠标位置
    QPoint lastPoint;
    QPoint movePoint;
};

CommonWidget::CommonWidget(QWidget *parent)
    : QWidget(parent)
    , d(new CommonWidgetPrivate(this))
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setupUI();
    resize(1000, 618);
}

CommonWidget::~CommonWidget()
{
}

void CommonWidget::setRestoreMaxButtonVisible(bool visible)
{
    d->maxButton->setVisible(visible);
    d->restoreButton->setVisible(visible);
}

void CommonWidget::setMinButtonVisible(bool visible)
{
    d->minButton->setVisible(visible);
}

void CommonWidget::setTitle(const QString &title)
{
    d->titleLabel->setText(title);
}

void CommonWidget::setIcon(const QIcon &icon)
{
    d->iconLabel->setPixmap(icon.pixmap(d->iconLabel->width(),
                                        d->iconLabel->height()));
}

void CommonWidget::setCentralWidget(QWidget *widget)
{
    QHBoxLayout *layout = new QHBoxLayout(d->centralWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(widget);
}

void CommonWidget::setTitleBar(QWidget *widget)
{
    QHBoxLayout *layout = new QHBoxLayout(d->titleBar);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(widget);
}

void CommonWidget::setShadowPadding(int shadowPadding)
{
    d->shadowPadding = shadowPadding;
}

void CommonWidget::mousePressEvent(QMouseEvent *event)
{
    if(!d->titleWidget->rect().contains(event->pos()))
        return;     //标题栏点击有效
    //读取坐鼠标点击坐标点
    d->lastPoint = event->globalPos();

    QWidget::mousePressEvent(event);
}

void CommonWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (isMaximized()) return;
    if(!d->lastPoint.isNull()){
        //把移动的点记录下来
        d->movePoint = event->globalPos() - d->lastPoint;
        d->lastPoint = event->globalPos();      //更新记录点
        move(pos() + d->movePoint);
    }

    QWidget::mouseMoveEvent(event);
}

void CommonWidget::mouseReleaseEvent(QMouseEvent *event)
{
    d->lastPoint = QPoint();
    QWidget::mouseReleaseEvent(event);
}

void CommonWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(!d->titleWidget->rect().contains(event->pos()))
        return;     //标题栏点击有效
    if(d->maxButton->isVisible())
        d->maxButton->click();
    else if(d->restoreButton->isVisible())
        d->restoreButton->click();

    QWidget::mouseDoubleClickEvent(event);
}

void CommonWidget::setupUI()
{
    d->titleWidget = titleWidget();
    d->titleWidget->setObjectName("titleWidget");

    QWidget *widget = new QWidget(this);
    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect(this);
    effect->setOffset(0, 0);          //设置向哪个方向产生阴影效果(dx,dy)，特别地，(0,0)代表向四周发散
    effect->setColor(Qt::gray);       //设置阴影颜色，也可以setColor(QColor(220,220,220))
    effect->setBlurRadius(d->shadowPadding);        //设定阴影的模糊半径，数值越大越模糊
    widget->setGraphicsEffect(effect);

    QGridLayout *layout = new QGridLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(d->titleWidget, 0, 0);
    layout->addWidget(d->centralWidget, 1, 0);
    layout->addWidget(new QSizeGrip(this), 1, 0, Qt::AlignRight | Qt::AlignBottom);

    d->layout->addWidget(widget);
}

QWidget *CommonWidget::titleWidget()
{
    QToolButton *closeButton = new QToolButton(this);
    closeButton->setObjectName("closeButton");
    closeButton->setToolTip(tr("Close"));

    connect(d->minButton, &QToolButton::clicked, this, &CommonWidget::showMinimized);
    connect(d->maxButton, &QToolButton::clicked, [=]{
        d->layout->setContentsMargins(0, 0, 0, 0);
        showMaximized();
        d->maxButton->hide();
        d->restoreButton->show();
    });
    connect(d->restoreButton, &QToolButton::clicked, [=]{
        d->layout->setContentsMargins(d->shadowPadding, d->shadowPadding, d->shadowPadding, d->shadowPadding);
        showNormal();
        d->maxButton->show();
        d->restoreButton->hide();
    });
    connect(closeButton, &QToolButton::clicked, this, &CommonWidget::aboutToclose);

    if(isMaximized()){
        d->maxButton->hide();
        d->restoreButton->show();
    }else{
        d->maxButton->show();
        d->restoreButton->hide();
    }

    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(5, 5, 5, 5);
    layout->setSpacing(10);
    layout->addWidget(d->iconLabel);
    layout->addWidget(d->titleLabel);
    layout->addStretch();
    layout->addWidget(d->titleBar);
    layout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Maximum, QSizePolicy::Maximum));
    layout->addWidget(d->minButton);
    layout->addWidget(d->maxButton);
    layout->addWidget(d->restoreButton);
    layout->addWidget(closeButton);

    return widget;
}

void CommonWidget::buildConnnect()
{
    connect(this, &CommonWidget::aboutToclose, this, &CommonWidget::close);
    connect(this, &CommonWidget::windowTitleChanged, d->titleLabel, &QLabel::setText);
    connect(this, &CommonWidget::windowIconChanged, d->iconLabel, &QLabel::setWindowIcon);
}

}
