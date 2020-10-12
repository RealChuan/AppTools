#include "mediawidget.h"

#include <QKeyEvent>

MediaWidget::MediaWidget(QWidget *parent)
    : QVideoWidget(parent)
{
    QPalette p = palette();
    p.setColor(QPalette::Window, Qt::black);
    setPalette(p);

    setAttribute(Qt::WA_OpaquePaintEvent);
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
}

void MediaWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape && isFullScreen()) {
        setFullScreen(false);
        event->accept();
    } else if (event->key() == Qt::Key_Enter && event->modifiers() & Qt::Key_Alt) {
        setFullScreen(!isFullScreen());
        event->accept();
    } else {
        QVideoWidget::keyPressEvent(event);
    }
}

void MediaWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    setFullScreen(!isFullScreen());
    event->accept();
}

void MediaWidget::mousePressEvent(QMouseEvent *event)
{
    emit playOrPause();
    QVideoWidget::mousePressEvent(event);
}
