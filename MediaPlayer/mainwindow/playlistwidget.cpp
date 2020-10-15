#include "playlistwidget.h"
#include "playlistmodel.h"

#include <QFileInfo>
#include <QMediaPlaylist>

class PlayListWidgetPrivate{
public:
    PlayListWidgetPrivate(QObject *parent)
        : owner(parent){
        mediaList = new QMediaPlaylist;
        playListModel = new PlaylistModel(owner);
        playListModel->setPlaylist(mediaList);
    }
    QObject *owner;
    QMediaPlaylist *mediaList;
    PlaylistModel *playListModel;
};

PlayListWidget::PlayListWidget(QWidget *parent)
    : QAbstractItemView(parent)
    , d_ptr(new PlayListWidgetPrivate(this))
{
    setModel(d_ptr->playListModel);
    setCurrentIndex(d_ptr->playListModel->index(d_ptr->mediaList->currentIndex(), 0));
    connect(this, &QAbstractItemView::activated, this, &PlayListWidget::onJump);
    connect(d_ptr->mediaList, &QMediaPlaylist::currentIndexChanged, this, &PlayListWidget::playlistPositionChanged);
}

PlayListWidget::~PlayListWidget()
{

}

QModelIndex PlayListWidget::indexAt(const QPoint &point) const
{
    return QModelIndex();
}

void PlayListWidget::scrollTo(const QModelIndex &index, QAbstractItemView::ScrollHint hint)
{

}

QRect PlayListWidget::visualRect(const QModelIndex &index) const
{
    return QRect();
}

QMediaPlaylist *PlayListWidget::mediaList()
{
    return d_ptr->mediaList;
}

bool isPlaylist(const QUrl &url) // Check for ".m3u" playlists.
{
    if (!url.isLocalFile())
        return false;
    const QFileInfo fileInfo(url.toLocalFile());
    return fileInfo.exists() && !fileInfo.suffix().compare(QLatin1String("m3u"), Qt::CaseInsensitive);
}

void PlayListWidget::addMedia(const QList<QUrl> &urls)
{
    for (auto &url: urls) {
        if (isPlaylist(url))
            d_ptr->mediaList->load(url);
        else
            d_ptr->mediaList->addMedia(url);

        qDebug() << "PlayListWidget::addMedia" << url;
    }
}

void PlayListWidget::onPrevious()
{
    d_ptr->mediaList->previous();
}

void PlayListWidget::onJump(const QModelIndex &index)
{
    if (!index.isValid())
        return;
    d_ptr->mediaList->setCurrentIndex(index.row());
    emit play();
}

void PlayListWidget::onNext()
{
    d_ptr->mediaList->next();
}

void PlayListWidget::playlistPositionChanged(int currentItem)
{
    //clearHistogram();
    setCurrentIndex(d_ptr->playListModel->index(currentItem, 0));
}

int PlayListWidget::horizontalOffset() const
{
    return 0;
}

bool PlayListWidget::isIndexHidden(const QModelIndex &index) const
{
    return false;
}

QModelIndex PlayListWidget::moveCursor(QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
    return QModelIndex();
}

void PlayListWidget::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags flags)
{

}

int PlayListWidget::verticalOffset() const
{
    return 0;
}

QRegion PlayListWidget::visualRegionForSelection(const QItemSelection &selection) const
{
    return QRegion();
}
