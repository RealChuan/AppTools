#include "commonwidget.h"

#include <QtWidgets>

class CommonWidgetPrivate{
public:
    CommonWidgetPrivate(QWidget *parent) : owner(parent){
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
    }
    QWidget *owner;
    QLabel *iconLabel;
    QLabel *titleLabel;
    QToolButton *minButton;
    QToolButton *maxButton;
    QToolButton *restoreButton;
    QWidget *centralWidget;
    QWidget *titleBar;

    //记录鼠标位置
    QPoint lastPoint;
    QPoint movePoint;
};

CommonWidget::CommonWidget(QWidget *parent) : QWidget(parent)
  , d(new CommonWidgetPrivate(this))
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_StyledBackground);
    setAttribute(Qt::WA_TranslucentBackground);
    setupUI();
    resize(1000, 618);
}

CommonWidget::~CommonWidget()
{
    delete d;
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
    d->iconLabel->setPixmap(icon.pixmap(d->iconLabel->width(), d->iconLabel->height()));
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

void CommonWidget::mousePressEvent(QMouseEvent *event)
{
    //读取坐鼠标点击坐标点
    d->lastPoint = event->globalPos();
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
}

void CommonWidget::mouseReleaseEvent(QMouseEvent *)
{
    d->lastPoint = QPoint();
}

void CommonWidget::setupUI()
{
    QGridLayout *layout = new QGridLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(titleWidget(), 0, 0);
    layout->addWidget(d->centralWidget, 1, 0);
    layout->addWidget(new QSizeGrip(this), 1, 0, Qt::AlignRight | Qt::AlignBottom);
}

QWidget *CommonWidget::titleWidget()
{
    QToolButton *closeButton = new QToolButton(this);
    closeButton->setObjectName("closeButton");
    closeButton->setToolTip(tr("Close"));

    connect(d->minButton, &QToolButton::clicked, this, &CommonWidget::showMinimized);
    connect(d->maxButton, &QToolButton::clicked, [=]{
        showMaximized();
        d->maxButton->hide();
        d->restoreButton->show();
    });
    connect(d->restoreButton, &QToolButton::clicked, [=]{
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
    widget->setObjectName("titleWidget");
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
