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
        playButton->setToolTip(QObject::tr("Play|Pause"));

        previousButton = new QToolButton(owner);
        previousButton->setEnabled(true);
        previousButton->setObjectName("PreviousButton");
        previousButton->setToolTip(QObject::tr("Previous"));

        nextButton = new QToolButton(owner);
        nextButton->setEnabled(true);
        nextButton->setObjectName("NextButton");
        nextButton->setToolTip(QObject::tr("Next"));

        muteButton = new QToolButton(owner);
        muteButton->setCheckable(true);
        muteButton->setObjectName("VolumeButton");
        muteButton->setToolTip(QObject::tr("Volume"));

        volumeSlider = new QSlider(Qt::Horizontal, owner);
        volumeSlider->setRange(0, 100);

        rateBox = new QComboBox(owner);
        rateBox->addItem("0.5x", QVariant(0.5));
        rateBox->addItem("1.0x", QVariant(1.0));
        rateBox->addItem("2.0x", QVariant(2.0));
        rateBox->setCurrentIndex(1);

        fullScreenBtn = new QToolButton(owner);
        fullScreenBtn->setCheckable(true);
        fullScreenBtn->setToolTip(QObject::tr("Full Screen"));
    }
    QWidget *owner;
    QToolButton *playButton;
    QToolButton *previousButton;
    QToolButton *nextButton;
    QToolButton *muteButton;
    QSlider *volumeSlider;
    QComboBox *rateBox;
    QToolButton *fullScreenBtn;

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

bool PlayControls::isFullScreenButtonChecked()
{
    return d_ptr->fullScreenBtn->isChecked();
}

void PlayControls::setState(QMediaPlayer::State state)
{
    if (state == d_ptr->playerState)
        return;
    d_ptr->playerState = state;

    switch (state) {
    case QMediaPlayer::StoppedState:
        d_ptr->playButton->setChecked(false);
        //d_ptr->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        break;
    case QMediaPlayer::PlayingState:
        d_ptr->playButton->setChecked(true);
        //d_ptr->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        break;
    case QMediaPlayer::PausedState:
        d_ptr->playButton->setChecked(false);
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
    switch (d_ptr->playerState) {
    case QMediaPlayer::StoppedState:
    case QMediaPlayer::PausedState:
        emit play();
        break;
    case QMediaPlayer::PlayingState:
        emit pause();
        break;
    default: break;
    }
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

void PlayControls::setFullScreenButtonChecked(bool checked)
{
    d_ptr->fullScreenBtn->setChecked(checked);
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
    layout->addWidget(d_ptr->fullScreenBtn);
    layout->addStretch();
}

void PlayControls::buildConnect()
{
    connect(d_ptr->playButton, &QToolButton::clicked, this, &PlayControls::playClicked);
    connect(d_ptr->previousButton, &QToolButton::clicked, this, &PlayControls::previous);
    connect(d_ptr->nextButton, &QToolButton::clicked, this, &PlayControls::next);
    connect(d_ptr->muteButton, &QToolButton::clicked, this, &PlayControls::muteClicked);
    connect(d_ptr->volumeSlider, &QSlider::valueChanged, this, &PlayControls::onVolumeSliderValueChanged);
    connect(d_ptr->rateBox, QOverload<int>::of(&QComboBox::activated), this, &PlayControls::updateRate);
    connect(d_ptr->fullScreenBtn, &QToolButton::clicked, this, &PlayControls::fullScreen);
}
