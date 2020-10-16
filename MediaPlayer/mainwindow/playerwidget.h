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
    void setCustomAudioRole(const QString &role);

    void onPlay(bool);
    void metaDataChanged();
    void previousClicked();
    void seek(int seconds);

    void statusChanged(QMediaPlayer::MediaStatus status);
    void stateChanged(QMediaPlayer::State state);
    void bufferingProgress(int progress);
    void videoAvailableChanged(bool available);

    void displayErrorMessage();
    
private:
    void setupUI();
    void buildConnect();
    void init();
    void setTrackInfo(const QString &info);
    void setStatusInfo(const QString &info);
    void handleCursor(QMediaPlayer::MediaStatus status);
    bool isPlayerAvailable() const;

    QScopedPointer<PlayerWidgetPrivate> d_ptr;
};

#endif // PLAYERWIDGET_H
