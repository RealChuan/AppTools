#include "playlistview.h"
#include "playlistmodel.h"

#include <QAbstractItemView>
#include <QFileInfo>
#include <QMediaPlaylist>
#include <QScrollBar>

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

PlayListView::PlayListView(QWidget *parent)
    : QListView(parent)
    , d_ptr(new PlayListWidgetPrivate(this))
{
    this->horizontalScrollBar()->setHidden(true);
    setModel(d_ptr->playListModel);
    setCurrentIndex(d_ptr->playListModel->index(d_ptr->mediaList->currentIndex(), 0));
    connect(this, &QAbstractItemView::activated, this, &PlayListView::onJump);
    connect(d_ptr->mediaList, &QMediaPlaylist::currentIndexChanged, this, &PlayListView::playlistPositionChanged);
}

PlayListView::~PlayListView()
{

}

QMediaPlaylist *PlayListView::mediaList()
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

void PlayListView::addMedia(const QList<QUrl> &urls)
{
    for (auto &url: urls) {
        if (isPlaylist(url))
            d_ptr->mediaList->load(url);
        else
            d_ptr->mediaList->addMedia(url);
    }
}

void PlayListView::onPrevious()
{
    d_ptr->mediaList->previous();
}

void PlayListView::onJump(const QModelIndex &index)
{
    if (!index.isValid())
        return;
    d_ptr->mediaList->setCurrentIndex(index.row());
    emit play();
}

void PlayListView::onNext()
{
    d_ptr->mediaList->next();
}

void PlayListView::playlistPositionChanged(int currentItem)
{
    //clearHistogram();
    setCurrentIndex(d_ptr->playListModel->index(currentItem, 0));
}
