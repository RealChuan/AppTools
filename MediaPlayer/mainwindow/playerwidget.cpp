#include "playerwidget.h"
#include "mediawidget.h"
#include "playcontrols.h"
#include "playlistwidget.h"

#include <QtWidgets>
#include <QMediaMetaData>

class PlayerWidgetPrivate{
public:
    PlayerWidgetPrivate(QWidget *parent)
        : owner(parent){
        playControls = new PlayControls(owner);
        playListWidget = new PlayListWidget(owner);
        mediaWidget = new MediaWidget(owner);
        mediaPlayer = new QMediaPlayer(owner);
        mediaPlayer->setAudioRole(QAudio::VideoRole);
        mediaPlayer->setVideoOutput(mediaWidget);
        mediaPlayer->setPlaylist(playListWidget->mediaList());

        playControls->setVolume(mediaPlayer->volume());
        playControls->setState(mediaPlayer->state());
        playControls->setMuted(mediaPlayer->isMuted());

        slider = new QSlider(Qt::Horizontal, owner);
        slider->setRange(0, mediaPlayer->duration() / 1000);
        durationLabel = new QLabel(owner);
    }
    QWidget *owner;
    QMediaPlayer *mediaPlayer;
    MediaWidget *mediaWidget;
    PlayControls *playControls;
    PlayListWidget *playListWidget;
    QSlider *slider;
    QLabel *durationLabel;

    QLabel *coverLabel = nullptr;

    QString trackInfo;
    QString statusInfo;
    qint64 duration;
};

PlayerWidget::PlayerWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new PlayerWidgetPrivate(this))
{
    qInfo() << "Supported audio roles:";
    for (QAudio::Role role : d_ptr->mediaPlayer->supportedAudioRoles())
        qInfo() << "    " << role;

    setupUI();
    buildConnect();

    if (!isPlayerAvailable()) {
        QMessageBox::warning(this, tr("Service not available"),
                             tr("The QMediaPlayer object does not have a valid service.\n"\
                                "Please check the media service plugins are installed."));

        d_ptr->playControls->setEnabled(false);
        d_ptr->playListWidget->setEnabled(false);
    }

    metaDataChanged();
}

PlayerWidget::~PlayerWidget()
{

}

void PlayerWidget::onPlay(bool play)
{
    if(play)
        d_ptr->mediaPlayer->play();
    else
        d_ptr->mediaPlayer->pause();
}

void PlayerWidget::durationChanged(qint64 duration)
{
    d_ptr->duration = duration / 1000;
    d_ptr->slider->setMaximum(d_ptr->duration);
}

void PlayerWidget::positionChanged(qint64 progress)
{
    if (!d_ptr->slider->isSliderDown())
        d_ptr->slider->setValue(progress / 1000);

    updateDurationInfo(progress / 1000);
}

void PlayerWidget::metaDataChanged()
{
    qDebug() << "PlayerWidget::metaDataChanged" << d_ptr->mediaPlayer;
    if (!d_ptr->mediaPlayer->isMetaDataAvailable())
        return;
    setTrackInfo(QString("%1 - %2")
                 .arg(d_ptr->mediaPlayer->metaData(QMediaMetaData::AlbumArtist).toString())
                 .arg(d_ptr->mediaPlayer->metaData(QMediaMetaData::Title).toString()));

    if (!d_ptr->coverLabel)
        return;
    QUrl url = d_ptr->mediaPlayer->metaData(QMediaMetaData::CoverArtUrlLarge).value<QUrl>();
    d_ptr->coverLabel->setPixmap(!url.isEmpty()
                                 ? QPixmap(url.toString())
                                 : QPixmap());
}

void PlayerWidget::previousClicked()
{
    // Go to previous track if we are within the first 5 seconds of playback
    // Otherwise, seek to the beginning.
    if (d_ptr->mediaPlayer->position() <= 5000)
        d_ptr->playListWidget->onPrevious();
    else
        d_ptr->mediaPlayer->setPosition(0);
}

void PlayerWidget::statusChanged(QMediaPlayer::MediaStatus status)
{
    handleCursor(status);

    // handle status message
    switch (status) {
    case QMediaPlayer::UnknownMediaStatus:
    case QMediaPlayer::NoMedia:
    case QMediaPlayer::LoadedMedia:
        setStatusInfo(QString());
        break;
    case QMediaPlayer::LoadingMedia:
        setStatusInfo(tr("Loading..."));
        break;
    case QMediaPlayer::BufferingMedia:
    case QMediaPlayer::BufferedMedia:
        setStatusInfo(tr("Buffering %1%").arg(d_ptr->mediaPlayer->bufferStatus()));
        break;
    case QMediaPlayer::StalledMedia:
        setStatusInfo(tr("Stalled %1%").arg(d_ptr->mediaPlayer->bufferStatus()));
        break;
    case QMediaPlayer::EndOfMedia:
        QApplication::alert(this);
        break;
    case QMediaPlayer::InvalidMedia:
        displayErrorMessage();
        break;
    }
}

void PlayerWidget::stateChanged(QMediaPlayer::State state)
{
    qDebug() << "PlayerWidget::stateChanged" << state;;
    d_ptr->playControls->setState(state);
    //if (state == QMediaPlayer::StoppedState)
    //   clearHistogram();
}

void PlayerWidget::bufferingProgress(int progress)
{
    if (d_ptr->mediaPlayer->mediaStatus() == QMediaPlayer::StalledMedia)
        setStatusInfo(tr("Stalled %1%").arg(progress));
    else
        setStatusInfo(tr("Buffering %1%").arg(progress));
}

void PlayerWidget::videoAvailableChanged(bool available)
{
    if (!available) {
        disconnect(d_ptr->playControls, &PlayControls::fullScreen, d_ptr->mediaWidget, &MediaWidget::setFullScreen);
        disconnect(d_ptr->mediaWidget, &MediaWidget::fullScreenChanged, d_ptr->playControls, &PlayControls::setFullScreenButtonChecked);
        d_ptr->mediaWidget->setFullScreen(false);
    } else {
        connect(d_ptr->playControls, &PlayControls::fullScreen, d_ptr->mediaWidget, &MediaWidget::setFullScreen);
        connect(d_ptr->mediaWidget, &QVideoWidget::fullScreenChanged, d_ptr->playControls, &PlayControls::setFullScreenButtonChecked);

        if (d_ptr->playControls->isFullScreenButtonChecked())
            d_ptr->mediaWidget->setFullScreen(true);
    }

    //m_colorButton->setEnabled(available);
}

void PlayerWidget::seek(int seconds)
{
    d_ptr->mediaPlayer->setPosition(seconds * 1000);
}

void PlayerWidget::displayErrorMessage()
{
    setStatusInfo(d_ptr->mediaPlayer->errorString());
}

void PlayerWidget::setupUI()
{    
    QHBoxLayout *sliderLayout = new QHBoxLayout;
    sliderLayout->addWidget(d_ptr->slider);
    sliderLayout->addWidget(d_ptr->durationLabel);

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addWidget(d_ptr->mediaWidget);
    leftLayout->addLayout(sliderLayout);
    leftLayout->addWidget(d_ptr->playControls);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addLayout(leftLayout);
    layout->addWidget(d_ptr->playListWidget);
}

void PlayerWidget::buildConnect()
{
    connect(d_ptr->mediaPlayer, &QMediaPlayer::durationChanged, this, &PlayerWidget::durationChanged);
    connect(d_ptr->mediaPlayer, &QMediaPlayer::positionChanged, this, &PlayerWidget::positionChanged);
    connect(d_ptr->mediaPlayer, QOverload<>::of(&QMediaPlayer::metaDataChanged), this, &PlayerWidget::metaDataChanged);
    connect(d_ptr->mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &PlayerWidget::statusChanged);
    connect(d_ptr->mediaPlayer, &QMediaPlayer::bufferStatusChanged, this, &PlayerWidget::bufferingProgress);
    connect(d_ptr->mediaPlayer, &QMediaPlayer::videoAvailableChanged, this, &PlayerWidget::videoAvailableChanged);
    connect(d_ptr->mediaPlayer, QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error), this, &PlayerWidget::displayErrorMessage);
    connect(d_ptr->mediaPlayer, &QMediaPlayer::stateChanged, this, &PlayerWidget::stateChanged);
    connect(d_ptr->mediaPlayer, &QMediaPlayer::volumeChanged, d_ptr->playControls, &PlayControls::setVolume);
    connect(d_ptr->mediaPlayer, &QMediaPlayer::mutedChanged, d_ptr->playControls, &PlayControls::setMuted);

    connect(d_ptr->slider, &QSlider::sliderMoved, this, &PlayerWidget::seek);

    connect(d_ptr->playControls, &PlayControls::play, d_ptr->mediaPlayer, &QMediaPlayer::play);
    connect(d_ptr->playControls, &PlayControls::pause, d_ptr->mediaPlayer, &QMediaPlayer::pause);
    connect(d_ptr->playControls, &PlayControls::stop, d_ptr->mediaPlayer, &QMediaPlayer::stop);
    connect(d_ptr->playControls, &PlayControls::next, d_ptr->playListWidget, &PlayListWidget::onNext);
    connect(d_ptr->playControls, &PlayControls::previous, this, &PlayerWidget::previousClicked);
    connect(d_ptr->playControls, &PlayControls::changeVolume, d_ptr->mediaPlayer, &QMediaPlayer::setVolume);
    connect(d_ptr->playControls, &PlayControls::changeMuting, d_ptr->mediaPlayer, &QMediaPlayer::setMuted);
    connect(d_ptr->playControls, &PlayControls::changeRate, d_ptr->mediaPlayer, &QMediaPlayer::setPlaybackRate);
    connect(d_ptr->playControls, &PlayControls::stop, d_ptr->mediaWidget, QOverload<>::of(&QVideoWidget::update));

    connect(d_ptr->mediaWidget, &MediaWidget::addMedia, d_ptr->playListWidget, &PlayListWidget::addMedia);
}

void PlayerWidget::init()
{

}

void PlayerWidget::setTrackInfo(const QString &info)
{
    d_ptr->trackInfo = info;
}

void PlayerWidget::setStatusInfo(const QString &info)
{
    d_ptr->statusInfo = info;
}

void PlayerWidget::handleCursor(QMediaPlayer::MediaStatus status)
{
#ifndef QT_NO_CURSOR
    if (status == QMediaPlayer::LoadingMedia ||
            status == QMediaPlayer::BufferingMedia ||
            status == QMediaPlayer::StalledMedia)
        setCursor(QCursor(Qt::BusyCursor));
    else
        unsetCursor();
#endif
}

void PlayerWidget::updateDurationInfo(qint64 currentInfo)
{
    QString tStr;
    if (currentInfo || d_ptr->duration) {
        QTime currentTime((currentInfo / 3600) % 60, (currentInfo / 60) % 60,
                          currentInfo % 60, (currentInfo * 1000) % 1000);
        QTime totalTime((d_ptr->duration / 3600) % 60, (d_ptr->duration / 60) % 60,
                        d_ptr->duration % 60, (d_ptr->duration * 1000) % 1000);
        QString format = "mm:ss";
        if (d_ptr->duration > 3600)
            format = "hh:mm:ss";
        tStr = currentTime.toString(format) + " / " + totalTime.toString(format);
    }
    d_ptr->durationLabel->setText(tStr);
}

bool PlayerWidget::isPlayerAvailable() const
{
    return d_ptr->mediaPlayer->isAvailable();
}
