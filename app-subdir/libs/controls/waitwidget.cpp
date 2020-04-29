#include "waitwidget.h"

#include <utils/utils.h>

#include <QtWidgets>

class WaitWidgetPrivate{
public:
    WaitWidgetPrivate(QWidget *parent) : owner(parent){
        processBar = new QProgressBar(owner);
        processBar->setTextVisible(false);
        processBar->setRange(0, 100);
    }
    QWidget *owner;
    QProgressBar *processBar;
    QTimer timer;
};

WaitWidget::WaitWidget(QWidget *parent) : QSplashScreen(parent)
  , d(new WaitWidgetPrivate(this))
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_StyledBackground);
    setAttribute(Qt::WA_TranslucentBackground);
    setupUI();
    connect(&d->timer, &QTimer::timeout, this, &WaitWidget::updateProgressBar);
    d->timer.start(200);
}

WaitWidget::~WaitWidget()
{
    delete d;
}

void WaitWidget::fullProgressBar()
{
    int value = d->processBar->value();
    if(value < 100)
        d->processBar->setValue(100);
}

void WaitWidget::updateProgressBar()
{
    int value = d->processBar->value() + 5;
    if(value >= 95) {
        d->timer.stop();
        return;
    }
    d->processBar->setValue(value);
}

void WaitWidget::setupUI()
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(d->processBar);
    resize(600, 5);
    Utils::windowCenter(this);
}
