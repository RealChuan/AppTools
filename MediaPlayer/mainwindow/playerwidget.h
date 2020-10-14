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

signals:
    
private slots:
    void durationChanged(qint64 duration);
    void positionChanged(qint64 progress);
    void metaDataChanged();

    void seek(int seconds);

    void statusChanged(QMediaPlayer::MediaStatus status);
    void bufferingProgress(int progress);
    void videoAvailableChanged(bool available);

    void displayErrorMessage();
    
private:
    void setupUI();
    void buildConnect();
    void setTrackInfo(const QString &info);
    void setStatusInfo(const QString &info);
    void handleCursor(QMediaPlayer::MediaStatus status);
    void updateDurationInfo(qint64 currentInfo);

    QScopedPointer<PlayerWidgetPrivate> d_ptr;
};

#endif // PLAYERWIDGET_H
