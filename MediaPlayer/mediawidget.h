#ifndef MEDIAWIDGET_H
#define MEDIAWIDGET_H

#include <QVideoWidget>

class MediaWidgetPrivate;
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

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    void buildConnect();

    QScopedPointer<MediaWidgetPrivate> d_ptr;
};

#endif // MEDIAWIDGET_H
