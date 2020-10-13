#include "playlistwidget.h"
#include "playlistmodel.h"

#include <QFileInfo>
#include <QMediaPlaylist>

class PLayListWidgetPrivate{
public:
    PLayListWidgetPrivate(QObject *parent)
        : owner(parent){
        mediaList = new QMediaPlaylist(owner);
        playListModel = new PlaylistModel(owner);
        playListModel->setPlaylist(mediaList);
    }
    QObject *owner;
    QMediaPlaylist *mediaList;
    PlaylistModel *playListModel;
};

PLayListWidget::PLayListWidget(QWidget *parent)
    : QAbstractItemView(parent)
    , d_ptr(new PLayListWidgetPrivate(this))
{
    setModel(d_ptr->playListModel);
    setCurrentIndex(d_ptr->playListModel->index(d_ptr->mediaList->currentIndex(), 0));
    connect(this, &QAbstractItemView::activated, this, &PLayListWidget::onJump);
}

PLayListWidget::~PLayListWidget()
{

}

QMediaPlaylist *PLayListWidget::mediaList()
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

void PLayListWidget::addMedia(const QList<QUrl> &urls)
{
    for (auto &url: urls) {
        if (isPlaylist(url))
            d_ptr->mediaList->load(url);
        else
            d_ptr->mediaList->addMedia(url);
    }
}

void PLayListWidget::onPervious()
{
    d_ptr->mediaList->previous();
}

void PLayListWidget::onJump(const QModelIndex &index)
{
    if (!index.isValid())
        return;
    d_ptr->mediaList->setCurrentIndex(index.row());
    emit play();
}

void PLayListWidget::onNext()
{
    d_ptr->mediaList->next();
}
