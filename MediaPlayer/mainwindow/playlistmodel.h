#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <QAbstractItemModel>

struct PlaylistModelPrivate;
class QMediaPlaylist;
class PlaylistModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    PlaylistModel(QObject *parent = nullptr);
    ~PlaylistModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::DisplayRole) override;

    QMediaPlaylist *playlist() const;
    void setPlaylist(QMediaPlaylist *playlist);

private slots:
    void beginInsertItems(int start, int end);
    void endInsertItems();
    void beginRemoveItems(int start, int end);
    void endRemoveItems();
    void changeItems(int start, int end);

private:
    QScopedPointer<PlaylistModelPrivate> d_ptr;
};

#endif // PLAYLISTMODEL_H
