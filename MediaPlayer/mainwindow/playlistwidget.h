#ifndef PLAYLISTWIDGET_H
#define PLAYLISTWIDGET_H

#include <QAbstractItemView>
#include <QUrl>

class QMediaPlaylist;
class PlayListWidgetPrivate;
class PlayListWidget : public QAbstractItemView
{
    Q_OBJECT
public:
    PlayListWidget(QWidget *parent = nullptr);
    ~PlayListWidget();

    virtual QModelIndex indexAt(const QPoint &point) const override;
    virtual void scrollTo(const QModelIndex &index,
                  QAbstractItemView::ScrollHint hint = EnsureVisible) override;
    virtual QRect visualRect(const QModelIndex &index) const override;

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

protected:
    virtual int horizontalOffset() const override;
    virtual bool isIndexHidden(const QModelIndex &index) const override;
    virtual QModelIndex moveCursor(QAbstractItemView::CursorAction cursorAction,
                           Qt::KeyboardModifiers modifiers) override;
    virtual void setSelection(const QRect &rect,
                      QItemSelectionModel::SelectionFlags flags) override;
    virtual int verticalOffset() const override;
    virtual QRegion visualRegionForSelection(const QItemSelection &selection) const override;

private:
    QScopedPointer<PlayListWidgetPrivate> d_ptr;
};

#endif // PLAYLISTWIDGET_H
