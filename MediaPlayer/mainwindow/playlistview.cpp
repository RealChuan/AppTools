#include "playlistview.h"
#include "playlistmodel.h"

#include <QAbstractItemView>
#include <QContextMenuEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QMenu>
#include <QScrollBar>
#include <QStandardPaths>

class PlayListWidgetPrivate{
public:
    PlayListWidgetPrivate(QWidget *parent)
        : owner(parent){
        mediaList = new QMediaPlaylist;
        QFile file("./config/MediaList.txt");
        file.open(QIODevice::ReadOnly);
        mediaList->load(&file, "%s");
        playListModel = new PlaylistModel(owner);
        playListModel->setPlaylist(mediaList);
        menu = new QMenu(owner);
    }
    QWidget *owner;
    QMediaPlaylist *mediaList;
    PlaylistModel *playListModel;
    QMenu *menu;
};

PlayListView::PlayListView(QWidget *parent)
    : QListView(parent)
    , d_ptr(new PlayListWidgetPrivate(this))
{
    horizontalScrollBar()->setHidden(true);
    setModel(d_ptr->playListModel);
    setCurrentIndex(d_ptr->playListModel->index(d_ptr->mediaList->currentIndex(), 0));
    connect(this, &QAbstractItemView::activated, this, &PlayListView::onJump);
    connect(d_ptr->mediaList, &QMediaPlaylist::currentIndexChanged, this, &PlayListView::playlistPositionChanged);
    initMenu();
}

PlayListView::~PlayListView()
{
    QFile file("./config/MediaList.txt");
    file.open(QIODevice::WriteOnly);
    d_ptr->mediaList->save(&file, "%s");
}

QMediaPlaylist *PlayListView::mediaList()
{
    return d_ptr->mediaList;
}

void PlayListView::onOpenMedia()
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
        addMedia(fileDialog.selectedUrls());
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
    emit playListChanged();
}

void PlayListView::onRemoveMedia()
{
    QModelIndex index = currentIndex();
    if(!index.isValid())
        return;
    int row = index.row();
    d_ptr->mediaList->removeMedia(row, row);
    emit playListChanged();
}

void PlayListView::onRemoveAllMedia()
{
    d_ptr->mediaList->clear();
    emit playListChanged();
}

void PlayListView::onPrevious()
{
    if(d_ptr->mediaList->isEmpty() || !d_ptr->mediaList->previousIndex())
        return;
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
    if(d_ptr->mediaList->isEmpty() || !d_ptr->mediaList->nextIndex())
        return;
    d_ptr->mediaList->next();
}

void PlayListView::playlistPositionChanged(int currentItem)
{
    //clearHistogram();
    setCurrentIndex(d_ptr->playListModel->index(currentItem, 0));
}

void PlayListView::contextMenuEvent(QContextMenuEvent *event)
{
    d_ptr->menu->exec(mapToGlobal(event->pos()));
}

void PlayListView::initMenu()
{
    d_ptr->menu->addAction(tr("Add Media File"), this, &PlayListView::onOpenMedia);
    d_ptr->menu->addAction(tr("Remove Media File"), this, &PlayListView::onRemoveMedia);
    d_ptr->menu->addAction(tr("Remove All Media File"), this, &PlayListView::onRemoveMedia);
}
