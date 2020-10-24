#include "mediawidget.h"

#include <utils/utils.h>

#include <QFileDialog>
#include <QKeyEvent>
#include <QMediaPlayer>
#include <QMenu>
#include <QStandardPaths>
#include <QMimeData>

MediaWidget::MediaWidget(QWidget *parent)
    : QVideoWidget(parent)
    , m_menu(new QMenu(this))
{
    QPalette p = palette();
    p.setColor(QPalette::Window, QColor(13,14,17));
    setPalette(p);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    setAcceptDrops(true);
    initMenu();
}

MediaWidget::~MediaWidget()
{

}

void MediaWidget::onFullScrren()
{
    setFullScreen(!isFullScreen());
}

void MediaWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape && isFullScreen()) {
        setFullScreen(false);
        event->accept();
    } else if (event->key() == Qt::Key_Enter && event->modifiers() & Qt::Key_Alt) {
        onFullScrren();
        event->accept();
    } else {
        QVideoWidget::keyPressEvent(event);
    }
}

void MediaWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    onFullScrren();
    event->accept();
}

void MediaWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
        emit play();
    QVideoWidget::mousePressEvent(event);
}

void MediaWidget::dragEnterEvent(QDragEnterEvent *event)
{
    QVideoWidget::dragEnterEvent(event);
    event->acceptProposedAction();
}

void MediaWidget::dragMoveEvent(QDragMoveEvent *event)
{
    QVideoWidget::dragMoveEvent(event);
    event->acceptProposedAction();
}

void MediaWidget::dropEvent(QDropEvent *event)
{
    QVideoWidget::dropEvent(event);
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty())
        return;
    emit addMediaUrls(urls);
}

void MediaWidget::contextMenuEvent(QContextMenuEvent *event)
{
    m_menu->exec(mapToGlobal(event->pos()));
}

void MediaWidget::initMenu()
{
    m_menu->addAction(tr("Open"), this, &MediaWidget::addMedia);
    m_menu->addSeparator();
    m_menu->addAction(tr("Previous"), this, &MediaWidget::previous);
    m_menu->addAction(tr("Next"), this, &MediaWidget::next);
    m_menu->addAction(tr("Double Screen"), this, &MediaWidget::doubleScreen);
    m_menu->addAction(tr("Full Screen"), this, &MediaWidget::onFullScrren);
    m_menu->addSeparator();
    m_menu->addAction(tr("Reload QSS"), []{ Utils::setQSS(); });
}
