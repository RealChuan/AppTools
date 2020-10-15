#include "playlistmodel.h"

#include <QFileInfo>
#include <QMediaPlaylist>

enum Column
{
    Title = 0,
    ColumnCount
};

struct PlaylistModelPrivate{
    QScopedPointer<QMediaPlaylist> playlist;
    QMap<QModelIndex, QVariant> dataMap;
};

PlaylistModel::PlaylistModel(QObject *parent)
    : QAbstractItemModel(parent)
    , d_ptr(new PlaylistModelPrivate)
{
}

PlaylistModel::~PlaylistModel()
{
    d_ptr->playlist->clear();
    d_ptr->dataMap.clear();
}

int PlaylistModel::rowCount(const QModelIndex &parent) const
{
    return d_ptr->playlist && !parent.isValid() ? d_ptr->playlist->mediaCount() : 0;
}

int PlaylistModel::columnCount(const QModelIndex &parent) const
{
    return !parent.isValid() ? ColumnCount : 0;
}

QModelIndex PlaylistModel::index(int row, int column, const QModelIndex &parent) const
{
    return d_ptr->playlist && !parent.isValid()
            && row >= 0 && row < d_ptr->playlist->mediaCount()
            && column >= 0 && column < ColumnCount
        ? createIndex(row, column)
        : QModelIndex();
}

QModelIndex PlaylistModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);
    return QModelIndex();
}

QVariant PlaylistModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid() && role == Qt::DisplayRole) {
        QVariant value = d_ptr->dataMap[index];
        if (!value.isValid() && index.column() == Title) {
            QUrl location = d_ptr->playlist->media(index.row()).request().url();
            return QFileInfo(location.path()).fileName();
        }
        return value;
    }
    return QVariant();
}

bool PlaylistModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(role);
    d_ptr->dataMap[index] = value;
    emit dataChanged(index, index);
    return true;
}

QMediaPlaylist *PlaylistModel::playlist() const
{
    return d_ptr->playlist.data();
}

void PlaylistModel::setPlaylist(QMediaPlaylist *playlist)
{
    if (d_ptr->playlist) {
        disconnect(d_ptr->playlist.data(), &QMediaPlaylist::mediaAboutToBeInserted, this, &PlaylistModel::beginInsertItems);
        disconnect(d_ptr->playlist.data(), &QMediaPlaylist::mediaInserted, this, &PlaylistModel::endInsertItems);
        disconnect(d_ptr->playlist.data(), &QMediaPlaylist::mediaAboutToBeRemoved, this, &PlaylistModel::beginRemoveItems);
        disconnect(d_ptr->playlist.data(), &QMediaPlaylist::mediaRemoved, this, &PlaylistModel::endRemoveItems);
        disconnect(d_ptr->playlist.data(), &QMediaPlaylist::mediaChanged, this, &PlaylistModel::changeItems);
    }

    beginResetModel();
    d_ptr->playlist.reset(playlist); // diconnect?

    if (d_ptr->playlist) {
        connect(d_ptr->playlist.data(), &QMediaPlaylist::mediaAboutToBeInserted, this, &PlaylistModel::beginInsertItems);
        connect(d_ptr->playlist.data(), &QMediaPlaylist::mediaInserted, this, &PlaylistModel::endInsertItems);
        connect(d_ptr->playlist.data(), &QMediaPlaylist::mediaAboutToBeRemoved, this, &PlaylistModel::beginRemoveItems);
        connect(d_ptr->playlist.data(), &QMediaPlaylist::mediaRemoved, this, &PlaylistModel::endRemoveItems);
        connect(d_ptr->playlist.data(), &QMediaPlaylist::mediaChanged, this, &PlaylistModel::changeItems);
    }

    endResetModel();
}

void PlaylistModel::beginInsertItems(int start, int end)
{
    d_ptr->dataMap.clear();
    beginInsertRows(QModelIndex(), start, end);
}

void PlaylistModel::endInsertItems()
{
    endInsertRows();
}

void PlaylistModel::beginRemoveItems(int start, int end)
{
    d_ptr->dataMap.clear();
    beginRemoveRows(QModelIndex(), start, end);
}

void PlaylistModel::endRemoveItems()
{
    endInsertRows();
}

void PlaylistModel::changeItems(int start, int end)
{
    d_ptr->dataMap.clear();
    emit dataChanged(index(start,0), index(end,ColumnCount));
}
