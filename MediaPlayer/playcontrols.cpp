#include "playcontrols.h"

#include <QtWidgets>

class PlayControlsPrivate{
public:
    PlayControlsPrivate(QWidget *parent)
        : owner(parent){
        playButton = new QToolButton(owner);
        playButton->setCheckable(true);
        playButton->setEnabled(true);
        playButton->setObjectName("PlayButton");
        playButton->setToolTip("Play|Pause");

        previousButton = new QToolButton(owner);
        previousButton->setEnabled(true);
        previousButton->setObjectName("PreviousButton");
        previousButton->setToolTip("Previous");

        nextButton = new QToolButton(owner);
        nextButton->setEnabled(true);
        nextButton->setObjectName("NextButton");
        nextButton->setToolTip("Next");

        muteButton = new QToolButton(owner);
        muteButton->setCheckable(true);
        muteButton->setObjectName("VolumeButton");
        muteButton->setToolTip("Volume");

        volumeSlider = new QSlider(Qt::Horizontal, owner);
        volumeSlider->setRange(0, 100);

        rateBox = new QComboBox(owner);
        rateBox->addItem("0.5x", QVariant(0.5));
        rateBox->addItem("1.0x", QVariant(1.0));
        rateBox->addItem("2.0x", QVariant(2.0));
        rateBox->setCurrentIndex(1);
    }
    QWidget *owner;
    QToolButton *playButton;
    QToolButton *previousButton;
    QToolButton *nextButton;
    QToolButton *muteButton;
    QSlider *volumeSlider;
    QComboBox *rateBox;

    QMediaPlayer::State playerState = QMediaPlayer::StoppedState;
    bool playerMuted = false;
};

PlayControls::PlayControls(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new PlayControlsPrivate(this))
{
    setupUI();
    buildConnect();
}

QMediaPlayer::State PlayControls::state() const
{
    return d_ptr->playerState;
}

int PlayControls::volume() const
{
    qreal linearVolume =  QAudio::convertVolume(d_ptr->volumeSlider->value() / qreal(100),
                                                QAudio::LogarithmicVolumeScale,
                                                QAudio::LinearVolumeScale);
    return qRound(linearVolume * 100);
}

bool PlayControls::isMuted() const
{
    return d_ptr->playerMuted;
}

qreal PlayControls::playbackRate() const
{
    return d_ptr->rateBox->itemData(d_ptr->rateBox->currentIndex()).toDouble();
}

void PlayControls::setState(QMediaPlayer::State state)
{
    if (state == d_ptr->playerState)
        return;
    d_ptr->playerState = state;

    switch (state) {
    case QMediaPlayer::StoppedState:
        //d_ptr->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        break;
    case QMediaPlayer::PlayingState:
        //d_ptr->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        break;
    case QMediaPlayer::PausedState:
        //d_ptr->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        break;
    }
}

void PlayControls::setVolume(int volume)
{
    qreal logarithmicVolume = QAudio::convertVolume(volume / qreal(100),
                                                    QAudio::LinearVolumeScale,
                                                    QAudio::LogarithmicVolumeScale);

    d_ptr->volumeSlider->setValue(qRound(logarithmicVolume * 100));
}

void PlayControls::setMuted(bool muted)
{
    if (muted == d_ptr->playerMuted)
        return;
    d_ptr->playerMuted = muted;
    //d_ptr->muteButton->setIcon(style()->standardIcon(muted
    //                                                 ? QStyle::SP_MediaVolumeMuted
    //                                                 : QStyle::SP_MediaVolume));
}

void PlayControls::setPlaybackRate(float rate)
{
    for (int i = 0; i < d_ptr->rateBox->count(); ++i) {
        if (qFuzzyCompare(rate, float(d_ptr->rateBox->itemData(i).toDouble()))) {
            d_ptr->rateBox->setCurrentIndex(i);
            return;
        }
    }

    d_ptr->rateBox->addItem(QString("%1x").arg(rate), QVariant(rate));
    d_ptr->rateBox->setCurrentIndex(d_ptr->rateBox->count() - 1);
}

void PlayControls::playClicked()
{
    bool played = false;
    switch (d_ptr->playerState) {
    case QMediaPlayer::StoppedState:
    case QMediaPlayer::PausedState:
        played = true;
        break;
    case QMediaPlayer::PlayingState:
        played = false;
        break;
    default: break;
    }
    emit play(played);
}

void PlayControls::muteClicked()
{
    emit changeMuting(!d_ptr->playerMuted);
}

void PlayControls::updateRate()
{
    emit changeRate(playbackRate());
}

void PlayControls::onVolumeSliderValueChanged()
{
    emit changeVolume(volume());
}

void PlayControls::setupUI()
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addStretch();
    layout->addWidget(d_ptr->previousButton);
    layout->addWidget(d_ptr->playButton);
    layout->addWidget(d_ptr->nextButton);
    layout->addWidget(d_ptr->rateBox);
    layout->addWidget(d_ptr->muteButton);
    layout->addWidget(d_ptr->volumeSlider);
    layout->addStretch();
}

void PlayControls::buildConnect()
{
    connect(d_ptr->playButton, &QToolButton::clicked, this, &PlayControls::play);
    connect(d_ptr->previousButton, &QToolButton::clicked, this, &PlayControls::previous);
    connect(d_ptr->nextButton, &QToolButton::clicked, this, &PlayControls::next);
    connect(d_ptr->muteButton, &QToolButton::clicked, this, &PlayControls::muteClicked);
    connect(d_ptr->volumeSlider, &QSlider::valueChanged, this, &PlayControls::onVolumeSliderValueChanged);
    connect(d_ptr->rateBox, QOverload<int>::of(&QComboBox::activated), this, &PlayControls::updateRate);
}
