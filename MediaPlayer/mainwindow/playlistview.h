#ifndef PLAYLISTVIEW_H
#define PLAYLISTVIEW_H

#include <QListView>
#include <QUrl>

class QMediaPlaylist;
class PlayListWidgetPrivate;
class PlayListView : public QListView
{
    Q_OBJECT
public:
    PlayListView(QWidget *parent = nullptr);
    ~PlayListView();

    QMediaPlaylist *mediaList();

signals:
    void play();
    void playListChanged();

public slots:
    void onOpenMedia();
    void addMedia(const QList<QUrl> &urls);
    void onRemoveMedia();
    void onRemoveAllMedia();
    void onPrevious();
    void onJump(const QModelIndex &index);
    void onNext();

private slots:
    void playlistPositionChanged(int);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    void initMenu();

    QScopedPointer<PlayListWidgetPrivate> d_ptr;
};

#endif // PLAYLISTVIEW_H
