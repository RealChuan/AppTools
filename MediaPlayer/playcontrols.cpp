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

}

void PlayControls::buildConnect()
{
    connect(d_ptr->playButton, &QToolButton::click, this, &PlayControls::play);
    connect(d_ptr->previousButton, &QToolButton::click, this, &PlayControls::previous);
    connect(d_ptr->nextButton, &QToolButton::click, this, &PlayControls::next);
    connect(d_ptr->muteButton, &QToolButton::click, this, &PlayControls::muteClicked);
    connect(d_ptr->volumeSlider, &QSlider::valueChanged, this, &PlayControls::onVolumeSliderValueChanged);
    connect(d_ptr->rateBox, QOverload<int>::of(&QComboBox::activated), this, &PlayControls::updateRate);



}
