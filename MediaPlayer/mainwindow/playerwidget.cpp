#include "playerwidget.h"
#include "mediawidget.h"
#include "playcontrolwidget.h"
#include "playlistview.h"

#include <QtWidgets>
#include <QMediaMetaData>

class PlayerWidgetPrivate{
public:
    PlayerWidgetPrivate(QWidget *parent)
        : owner(parent)
        , mediaPlayer(new QMediaPlayer)
    {
        playControls = new PlayControlWidget(owner);
        playListWidget = new PlayListView(owner);
        mediaWidget = new MediaWidget(owner);
        mediaPlayer->setAudioRole(QAudio::VideoRole);
        mediaPlayer->setVideoOutput(mediaWidget);
        mediaPlayer->setPlaylist(playListWidget->mediaList());

        playControls->setProcessValue(mediaPlayer->duration());
        playControls->setVolume(mediaPlayer->volume());
        playControls->setState(mediaPlayer->state());
        playControls->setMuted(mediaPlayer->isMuted());
    }
    QWidget *owner;
    QScopedPointer<QMediaPlayer> mediaPlayer;
    MediaWidget *mediaWidget;
    PlayControlWidget *playControls;
    PlayListView *playListWidget;

    QLabel *coverLabel = nullptr;

    QString trackInfo;
    QString statusInfo;
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

void PlayerWidget::setCustomAudioRole(const QString &role)
{
    d_ptr->mediaPlayer->setCustomAudioRole(role);
}

void PlayerWidget::onPlay(bool play)
{
    if(play)
        d_ptr->mediaPlayer->play();
    else
        d_ptr->mediaPlayer->pause();
}

void PlayerWidget::metaDataChanged()
{
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
        disconnect(d_ptr->playControls, &PlayControlWidget::fullScreen, d_ptr->mediaWidget, &MediaWidget::setFullScreen);
        disconnect(d_ptr->mediaWidget, &MediaWidget::fullScreenChanged, d_ptr->playControls, &PlayControlWidget::setFullScreenButtonChecked);
        d_ptr->mediaWidget->setFullScreen(false);
    } else {
        connect(d_ptr->playControls, &PlayControlWidget::fullScreen, d_ptr->mediaWidget, &MediaWidget::setFullScreen);
        connect(d_ptr->mediaWidget, &QVideoWidget::fullScreenChanged, d_ptr->playControls, &PlayControlWidget::setFullScreenButtonChecked);

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
    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(0);
    leftLayout->addWidget(d_ptr->mediaWidget);
    leftLayout->addWidget(d_ptr->playControls);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addLayout(leftLayout);
    layout->addWidget(d_ptr->playListWidget);
}

void PlayerWidget::buildConnect()
{
    connect(d_ptr->mediaPlayer.data(), QOverload<>::of(&QMediaPlayer::metaDataChanged), this, &PlayerWidget::metaDataChanged);
    connect(d_ptr->mediaPlayer.data(), &QMediaPlayer::mediaStatusChanged, this, &PlayerWidget::statusChanged);
    connect(d_ptr->mediaPlayer.data(), &QMediaPlayer::bufferStatusChanged, this, &PlayerWidget::bufferingProgress);
    connect(d_ptr->mediaPlayer.data(), &QMediaPlayer::videoAvailableChanged, this, &PlayerWidget::videoAvailableChanged);
    connect(d_ptr->mediaPlayer.data(), QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error), this, &PlayerWidget::displayErrorMessage);
    connect(d_ptr->mediaPlayer.data(), &QMediaPlayer::stateChanged, this, &PlayerWidget::stateChanged);
    connect(d_ptr->mediaPlayer.data(), &QMediaPlayer::volumeChanged, d_ptr->playControls, &PlayControlWidget::setVolume);
    connect(d_ptr->mediaPlayer.data(), &QMediaPlayer::mutedChanged, d_ptr->playControls, &PlayControlWidget::setMuted);
    connect(d_ptr->mediaPlayer.data(), &QMediaPlayer::durationChanged, d_ptr->playControls, &PlayControlWidget::durationChanged);
    connect(d_ptr->mediaPlayer.data(), &QMediaPlayer::positionChanged, d_ptr->playControls, &PlayControlWidget::positionChanged);

    connect(d_ptr->playControls, &PlayControlWidget::seek, this, &PlayerWidget::seek);
    connect(d_ptr->playControls, &PlayControlWidget::play, d_ptr->mediaPlayer.data(), &QMediaPlayer::play);
    connect(d_ptr->playControls, &PlayControlWidget::pause, d_ptr->mediaPlayer.data(), &QMediaPlayer::pause);
    connect(d_ptr->playControls, &PlayControlWidget::stop, d_ptr->mediaPlayer.data(), &QMediaPlayer::stop);
    connect(d_ptr->playControls, &PlayControlWidget::next, d_ptr->playListWidget, &PlayListView::onNext);
    connect(d_ptr->playControls, &PlayControlWidget::previous, this, &PlayerWidget::previousClicked);
    connect(d_ptr->playControls, &PlayControlWidget::changeVolume, d_ptr->mediaPlayer.data(), &QMediaPlayer::setVolume);
    connect(d_ptr->playControls, &PlayControlWidget::changeMuting, d_ptr->mediaPlayer.data(), &QMediaPlayer::setMuted);
    connect(d_ptr->playControls, &PlayControlWidget::changeRate, d_ptr->mediaPlayer.data(), &QMediaPlayer::setPlaybackRate);
    connect(d_ptr->playControls, &PlayControlWidget::stop, d_ptr->mediaWidget, QOverload<>::of(&QVideoWidget::update));

    connect(d_ptr->mediaWidget, &MediaWidget::addMedia, d_ptr->playListWidget, &PlayListView::addMedia);
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

bool PlayerWidget::isPlayerAvailable() const
{
    return d_ptr->mediaPlayer->isAvailable();
}
