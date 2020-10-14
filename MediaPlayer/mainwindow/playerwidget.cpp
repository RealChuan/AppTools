#include "playerwidget.h"
#include "mediawidget.h"
#include "playcontrols.h"

#include <QtWidgets>
#include <QMediaMetaData>

class PlayerWidgetPrivate{
public:
    PlayerWidgetPrivate(QWidget *parent)
        : owner(parent){
        mediaWidget = new MediaWidget(owner);
        meidaPalyer = new QMediaPlayer(owner);
        meidaPalyer->setAudioRole(QAudio::VideoRole);
        meidaPalyer->setVideoOutput(mediaWidget);

        slider = new QSlider(Qt::Horizontal, owner);
        slider->setRange(0, meidaPalyer->duration() / 1000);
        durationLabel = new QLabel(owner);

        playControls = new PlayControls(owner);
    }
    QWidget *owner;
    QMediaPlayer *meidaPalyer;
    MediaWidget *mediaWidget;
    QSlider *slider;
    QLabel *durationLabel;

    PlayControls *playControls;

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
    for (QAudio::Role role : d_ptr->meidaPalyer->supportedAudioRoles())
        qInfo() << "    " << role;

    setupUI();
    buildConnect();
}

PlayerWidget::~PlayerWidget()
{

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
    if (!d_ptr->meidaPalyer->isMetaDataAvailable())
        return;
    setTrackInfo(QString("%1 - %2")
                 .arg(d_ptr->meidaPalyer->metaData(QMediaMetaData::AlbumArtist).toString())
                 .arg(d_ptr->meidaPalyer->metaData(QMediaMetaData::Title).toString()));

    if (!d_ptr->coverLabel)
        return;
    QUrl url = d_ptr->meidaPalyer->metaData(QMediaMetaData::CoverArtUrlLarge).value<QUrl>();
    d_ptr->coverLabel->setPixmap(!url.isEmpty()
                                 ? QPixmap(url.toString())
                                 : QPixmap());
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
        setStatusInfo(tr("Buffering %1%").arg(d_ptr->meidaPalyer->bufferStatus()));
        break;
    case QMediaPlayer::StalledMedia:
        setStatusInfo(tr("Stalled %1%").arg(d_ptr->meidaPalyer->bufferStatus()));
        break;
    case QMediaPlayer::EndOfMedia:
        QApplication::alert(this);
        break;
    case QMediaPlayer::InvalidMedia:
        displayErrorMessage();
        break;
    }
}

void PlayerWidget::bufferingProgress(int progress)
{
    if (d_ptr->meidaPalyer->mediaStatus() == QMediaPlayer::StalledMedia)
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
    d_ptr->meidaPalyer->setPosition(seconds * 1000);
}

void PlayerWidget::displayErrorMessage()
{
    setStatusInfo(d_ptr->meidaPalyer->errorString());
}

void PlayerWidget::setupUI()
{

}

void PlayerWidget::buildConnect()
{
    connect(d_ptr->meidaPalyer, &QMediaPlayer::durationChanged, this, &PlayerWidget::durationChanged);
    connect(d_ptr->meidaPalyer, &QMediaPlayer::positionChanged, this, &PlayerWidget::positionChanged);
    connect(d_ptr->meidaPalyer, QOverload<>::of(&QMediaPlayer::metaDataChanged), this, &PlayerWidget::metaDataChanged);
    //connect(m_playlist, &QMediaPlaylist::currentIndexChanged, this, &Player::playlistPositionChanged);
    connect(d_ptr->meidaPalyer, &QMediaPlayer::mediaStatusChanged, this, &PlayerWidget::statusChanged);
    connect(d_ptr->meidaPalyer, &QMediaPlayer::bufferStatusChanged, this, &PlayerWidget::bufferingProgress);
    connect(d_ptr->meidaPalyer, &QMediaPlayer::videoAvailableChanged, this, &PlayerWidget::videoAvailableChanged);
    //connect(d_ptr->meidaPalyer, QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error), this, &Player::displayErrorMessage);
    //connect(d_ptr->meidaPalyer, &QMediaPlayer::stateChanged, this, &Player::stateChanged);

    connect(d_ptr->slider, &QSlider::sliderMoved, this, &PlayerWidget::seek);



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
