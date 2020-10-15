#include "mediawidget.h"

#include <QFileDialog>
#include <QKeyEvent>
#include <QMediaPlayer>
#include <QMenu>
#include <QStandardPaths>

MediaWidget::MediaWidget(QWidget *parent)
    : QVideoWidget(parent)
    , m_menu(new QMenu(this))
{
    QPalette p = palette();
    p.setColor(QPalette::Window, Qt::black);
    setPalette(p);

    setAttribute(Qt::WA_OpaquePaintEvent);
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    initMenu();
}

MediaWidget::~MediaWidget()
{

}

void MediaWidget::open()
{
    QFileDialog fileDialog(this);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setWindowTitle(tr("Open Files"));
    QStringList supportedMimeTypes = QMediaPlayer::supportedMimeTypes();
    if (!supportedMimeTypes.isEmpty()) {
        supportedMimeTypes.append("audio/x-m3u"); // MP3 playlists
        fileDialog.setMimeTypeFilters(supportedMimeTypes);
    }
    fileDialog.setDirectory(QStandardPaths::standardLocations(QStandardPaths::MoviesLocation).value(0, QDir::homePath()));
    if (fileDialog.exec() == QDialog::Accepted)
        emit addMedia(fileDialog.selectedUrls());
    //addToPlaylist(fileDialog.selectedUrls());
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

void MediaWidget::contextMenuEvent(QContextMenuEvent *event)
{
    m_menu->exec(mapToGlobal(event->pos()));
}

void MediaWidget::initMenu()
{
    m_menu->addAction(tr("Open"), this, &MediaWidget::open);
}
