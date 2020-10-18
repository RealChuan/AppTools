#ifndef PLAYCONTROLWIDGET_H
#define PLAYCONTROLWIDGET_H

#include <QWidget>
#include <QMediaPlayer>

class PlayControlWidgetPrivate;
class PlayControlWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlayControlWidget(QWidget *parent = nullptr);

    void setPlayButtonEnable(bool enable);
    QMediaPlayer::State state() const;
    int volume() const;
    bool isMuted() const;
    qreal playbackRate() const;

public slots:
    void onPlay();

    void setProcessValue(int offset);
    void durationChanged(qint64 duration);
    void positionChanged(qint64 progress);

    void setState(QMediaPlayer::State state);
    void setVolume(int volume);
    void setMuted(bool muted);
    void setPlaybackRate(float rate);

signals:
    void play();
    void pause();
    void stop();
    void next();
    void previous();
    void seek(int seconds);
    void changeVolume(int volume);
    void changeMuting(bool muting);
    void changeRate(qreal rate);
    void showList();

private slots:
    void onMuted(bool);
    void updateRate();
    void onVolumeSliderValueChanged();

protected:
    void paintEvent(QPaintEvent *event) override; // setStyleSheet 无效 解决方法

private:
    void setupUI();
    void buildConnect();
    void updateDurationInfo(qint64 currentInfo);

    QScopedPointer<PlayControlWidgetPrivate> d_ptr;
};

#endif // PLAYCONTROLWIDGET_H
