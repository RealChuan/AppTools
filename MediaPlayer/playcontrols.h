#ifndef PLAYCONTROLS_H
#define PLAYCONTROLS_H

#include <QWidget>
#include <QMediaPlayer>

class PlayControlsPrivate;
class PlayControls : public QWidget
{
    Q_OBJECT
public:
    explicit PlayControls(QWidget *parent = nullptr);

    QMediaPlayer::State state() const;
    int volume() const;
    bool isMuted() const;
    qreal playbackRate() const;

public slots:
    void setState(QMediaPlayer::State state);
    void setVolume(int volume);
    void setMuted(bool muted);
    void setPlaybackRate(float rate);

signals:
    void play(bool);
    void stop();
    void next();
    void previous();
    void changeVolume(int volume);
    void changeMuting(bool muting);
    void changeRate(qreal rate);

private slots:
    void playClicked();
    void muteClicked();
    void updateRate();
    void onVolumeSliderValueChanged();

private:
    void setupUI();
    void buildConnect();

    QScopedPointer<PlayControlsPrivate> d_ptr;
};

#endif // PLAYCONTROLS_H
