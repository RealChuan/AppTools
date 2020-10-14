#ifndef PLAYLISTWIDGET_H
#define PLAYLISTWIDGET_H

#include <QAbstractItemView>
#include <QUrl>

class QMediaPlaylist;
class PLayListWidgetPrivate;
class PLayListWidget : public QAbstractItemView
{
    Q_OBJECT
public:
    PLayListWidget(QWidget *parent = nullptr);
    ~PLayListWidget();

    QMediaPlaylist *mediaList();

signals:
    void play();

public slots:
    void addMedia(const QList<QUrl> &urls);
    void onPervious();
    void onJump(const QModelIndex &index);
    void onNext();

private:
    QScopedPointer<PLayListWidgetPrivate> d_ptr;
};

#endif // PLAYLISTWIDGET_H
