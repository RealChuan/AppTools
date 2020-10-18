#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H

#include <QMediaPlayer>
#include <QWidget>

class PlayerWidgetPrivate;
class PlayerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlayerWidget(QWidget *parent = nullptr);
    ~PlayerWidget();
    
private slots:
    void onPrevious();
    void onPlay(bool);
    void onDoubleScreen();
    void onSeek(int seconds);
    void metaDataChanged();

    void statusChanged(QMediaPlayer::MediaStatus status);
    void stateChanged(QMediaPlayer::State state);
    void bufferingProgress(int progress);

    void setPlayListVisiable();
    void setCustomAudioRole(const QString &role);

    void displayErrorMessage();
    void setPlayButtonEnable();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    
private:
    void setupUI();
    void buildConnect();
    void setTrackInfo(const QString &info);
    void setStatusInfo(const QString &info);
    void handleCursor(QMediaPlayer::MediaStatus status);
    bool isPlayerAvailable() const;

    QScopedPointer<PlayerWidgetPrivate> d_ptr;
};

#endif // PLAYERWIDGET_H
