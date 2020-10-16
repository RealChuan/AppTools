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

    QMediaPlayer::State state() const;
    int volume() const;
    bool isMuted() const;
    qreal playbackRate() const;
    bool isFullScreenButtonChecked();

public slots:
    void setProcessValue(int offset);
    void durationChanged(qint64 duration);
    void positionChanged(qint64 progress);

    void setState(QMediaPlayer::State state);
    void setVolume(int volume);
    void setMuted(bool muted);
    void setPlaybackRate(float rate);
    void setFullScreenButtonChecked(bool);

signals:
    void seek(int seconds);
    void play();
    void pause();
    void stop();
    void next();
    void previous();
    void changeVolume(int volume);
    void changeMuting(bool muting);
    void changeRate(qreal rate);
    void fullScreen(bool);

private slots:
    void playClicked();
    void muteClicked();
    void updateRate();
    void onVolumeSliderValueChanged();

private:
    void setupUI();
    void buildConnect();
    void updateDurationInfo(qint64 currentInfo);

    QScopedPointer<PlayControlWidgetPrivate> d_ptr;
};

#endif // PLAYCONTROLWIDGET_H
