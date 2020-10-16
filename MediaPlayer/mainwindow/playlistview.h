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

public slots:
    void addMedia(const QList<QUrl> &urls);
    void onPrevious();
    void onJump(const QModelIndex &index);
    void onNext();

private slots:
    void playlistPositionChanged(int);

private:
    QScopedPointer<PlayListWidgetPrivate> d_ptr;
};

#endif // PLAYLISTVIEW_H
