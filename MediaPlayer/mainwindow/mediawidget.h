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
    void playOrPause();
    void durationChanged(qint64 duration);
    void positionChanged(qint64 position);
    void addMedia(const QList<QUrl> &urls);

private slots:
    void open();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    void initMenu();

    QMenu *m_menu;
};

#endif // MEDIAWIDGET_H
