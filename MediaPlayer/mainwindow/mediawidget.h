#ifndef MEDIAWIDGET_H
#define MEDIAWIDGET_H

#include <QUrl>
#include <QVideoWidget>

class QMenu;
class MediaWidget : public QVideoWidget
{
    Q_OBJECT
public:
    MediaWidget(QWidget *parent = nullptr);
    ~MediaWidget();

signals:
    void doubleScreen();
    void previous();
    void next();
    void play();
    void durationChanged(qint64 duration);
    void positionChanged(qint64 position);
    void addMedia();
    void addMediaUrls(const QList<QUrl>&);

private slots:
    void onFullScrren();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    void initMenu();

    QMenu *m_menu;
};

#endif // MEDIAWIDGET_H
