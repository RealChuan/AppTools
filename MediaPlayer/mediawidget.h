#ifndef MEDIAWIDGET_H
#define MEDIAWIDGET_H

#include <QVideoWidget>

class MediaWidget : public QVideoWidget
{
    Q_OBJECT
public:
    MediaWidget(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void playOrPause();
};

#endif // MEDIAWIDGET_H
