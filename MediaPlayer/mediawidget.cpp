#include "mediawidget.h"

#include <QKeyEvent>
#include <QMediaPlayer>

class MediaWidgetPrivate{
public:
    MediaWidgetPrivate(QWidget *parent)
        : owner(parent){
        meidaPalyer = new QMediaPlayer(owner);
        meidaPalyer->setAudioRole(QAudio::VideoRole);
        qInfo() << "Supported audio roles:";
        for (QAudio::Role role : meidaPalyer->supportedAudioRoles())
            qInfo() << "    " << role;
    }
    QWidget *owner;
    QMediaPlayer *meidaPalyer;
};

MediaWidget::MediaWidget(QWidget *parent)
    : QVideoWidget(parent)
    , d_ptr(new MediaWidgetPrivate(this))
{
    QPalette p = palette();
    p.setColor(QPalette::Window, Qt::black);
    setPalette(p);

    setAttribute(Qt::WA_OpaquePaintEvent);
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    d_ptr->meidaPalyer->setVideoOutput(this);
}

MediaWidget::~MediaWidget()
{

}

void MediaWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape && isFullScreen()) {
        setFullScreen(false);
        event->accept();
    } else if (event->key() == Qt::Key_Enter && event->modifiers() & Qt::Key_Alt) {
        setFullScreen(!isFullScreen());
        event->accept();
    } else {
        QVideoWidget::keyPressEvent(event);
    }
}

void MediaWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    setFullScreen(!isFullScreen());
    event->accept();
}

void MediaWidget::mousePressEvent(QMouseEvent *event)
{
    emit playOrPause();
    QVideoWidget::mousePressEvent(event);
}

void MediaWidget::buildConnect()
{
    connect(d_ptr->meidaPalyer, &QMediaPlayer::durationChanged, this, &MediaWidget::durationChanged);
    connect(d_ptr->meidaPalyer, &QMediaPlayer::positionChanged, this, &MediaWidget::positionChanged);
    //connect(d_ptr->meidaPalyer, QOverload<>::of(&QMediaPlayer::metaDataChanged), this, &MediaWidget::metaDataChanged);
    //connect(d_ptr->meidaPalyer, &QMediaPlayer::mediaStatusChanged, this, &Player::statusChanged);
    //connect(d_ptr->meidaPalyer, &QMediaPlayer::bufferStatusChanged, this, &Player::bufferingProgress);
    //connect(d_ptr->meidaPalyer, &QMediaPlayer::videoAvailableChanged, this, &Player::videoAvailableChanged);
    //connect(d_ptr->meidaPalyer, QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error), this, &Player::displayErrorMessage);
    //connect(d_ptr->meidaPalyer, &QMediaPlayer::stateChanged, this, &Player::stateChanged);
}
