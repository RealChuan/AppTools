#include "playcontrolwidget.h"

#include <QtWidgets>

class PlayControlWidgetPrivate{
public:
    PlayControlWidgetPrivate(QWidget *parent)
        : owner(parent){
        progressSlider = new QSlider(Qt::Horizontal, owner);
        durationLabel = new QLabel(owner);
        totalTimeLabel = new QLabel(owner);

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
        fullScreenBtn->setObjectName("FullScreenButton");
        fullScreenBtn->setToolTip(QObject::tr("Full Screen"));
    }
    QWidget *owner;
    QSlider *progressSlider;
    QLabel *durationLabel;
    QLabel *totalTimeLabel;
    QToolButton *playButton;
    QToolButton *previousButton;
    QToolButton *nextButton;
    QToolButton *muteButton;
    QSlider *volumeSlider;
    QComboBox *rateBox;
    QToolButton *fullScreenBtn;

    qint64 totalTime;
    QMediaPlayer::State playerState = QMediaPlayer::StoppedState;
    bool playerMuted = false;
};

PlayControlWidget::PlayControlWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new PlayControlWidgetPrivate(this))
{
    setupUI();
    buildConnect();
}

QMediaPlayer::State PlayControlWidget::state() const
{
    return d_ptr->playerState;
}

int PlayControlWidget::volume() const
{
    qreal linearVolume =  QAudio::convertVolume(d_ptr->volumeSlider->value() / qreal(100),
                                                QAudio::LogarithmicVolumeScale,
                                                QAudio::LinearVolumeScale);
    return qRound(linearVolume * 100);
}

bool PlayControlWidget::isMuted() const
{
    return d_ptr->playerMuted;
}

qreal PlayControlWidget::playbackRate() const
{
    return d_ptr->rateBox->itemData(d_ptr->rateBox->currentIndex()).toDouble();
}

bool PlayControlWidget::isFullScreenButtonChecked()
{
    return d_ptr->fullScreenBtn->isChecked();
}

void PlayControlWidget::setProcessValue(int offset)
{
    d_ptr->progressSlider->setRange(0, offset);
}

void PlayControlWidget::durationChanged(qint64 duration)
{
    d_ptr->totalTime = duration / 1000;
    d_ptr->progressSlider->setMaximum(d_ptr->totalTime);
    QTime totalTime((d_ptr->totalTime / 3600) % 60, (d_ptr->totalTime / 60) % 60,
                    d_ptr->totalTime % 60, (d_ptr->totalTime * 1000) % 1000);
    QString format = "mm:ss";
    if (d_ptr->totalTime > 3600)
        format = "hh:mm:ss";
    d_ptr->totalTimeLabel->setText(totalTime.toString(format));
}

void PlayControlWidget::positionChanged(qint64 progress)
{
    if (!d_ptr->progressSlider->isSliderDown())
        d_ptr->progressSlider->setValue(progress / 1000);

    updateDurationInfo(progress / 1000);
}

void PlayControlWidget::setState(QMediaPlayer::State state)
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

void PlayControlWidget::setVolume(int volume)
{
    qreal logarithmicVolume = QAudio::convertVolume(volume / qreal(100),
                                                    QAudio::LinearVolumeScale,
                                                    QAudio::LogarithmicVolumeScale);

    d_ptr->volumeSlider->setValue(qRound(logarithmicVolume * 100));
}

void PlayControlWidget::setMuted(bool muted)
{
    if (muted == d_ptr->playerMuted)
        return;
    d_ptr->playerMuted = muted;
    //d_ptr->muteButton->setIcon(style()->standardIcon(muted
    //                                                 ? QStyle::SP_MediaVolumeMuted
    //                                                 : QStyle::SP_MediaVolume));
}

void PlayControlWidget::setPlaybackRate(float rate)
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

void PlayControlWidget::playClicked()
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

void PlayControlWidget::muteClicked()
{
    emit changeMuting(!d_ptr->playerMuted);
}

void PlayControlWidget::updateRate()
{
    emit changeRate(playbackRate());
}

void PlayControlWidget::onVolumeSliderValueChanged()
{
    emit changeVolume(volume());
}

void PlayControlWidget::setFullScreenButtonChecked(bool checked)
{
    d_ptr->fullScreenBtn->setChecked(checked);
}

void PlayControlWidget::setupUI()
{
    QHBoxLayout *durationLayout = new QHBoxLayout;
    durationLayout->setSpacing(0);
    durationLayout->setContentsMargins(0, 0, 0, 0);
    durationLayout->addWidget(d_ptr->durationLabel);
    durationLayout->addStretch();
    durationLayout->addWidget(d_ptr->totalTimeLabel);

    QHBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setSpacing(10);
    controlLayout->setContentsMargins(0, 0, 0, 0);
    controlLayout->addStretch();
    controlLayout->addWidget(d_ptr->previousButton);
    controlLayout->addWidget(d_ptr->playButton);
    controlLayout->addWidget(d_ptr->nextButton);
    controlLayout->addWidget(d_ptr->rateBox);
    controlLayout->addWidget(d_ptr->muteButton);
    controlLayout->addWidget(d_ptr->volumeSlider);
    controlLayout->addStretch();
    controlLayout->addWidget(d_ptr->fullScreenBtn);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 0, 20, 10);
    layout->setSpacing(0);
    layout->addWidget(d_ptr->progressSlider);
    layout->addLayout(durationLayout);
    layout->addLayout(controlLayout);
}

void PlayControlWidget::buildConnect()
{
    connect(d_ptr->progressSlider, &QSlider::sliderMoved, this, &PlayControlWidget::seek);
    connect(d_ptr->playButton, &QToolButton::clicked, this, &PlayControlWidget::playClicked);
    connect(d_ptr->previousButton, &QToolButton::clicked, this, &PlayControlWidget::previous);
    connect(d_ptr->nextButton, &QToolButton::clicked, this, &PlayControlWidget::next);
    connect(d_ptr->muteButton, &QToolButton::clicked, this, &PlayControlWidget::muteClicked);
    connect(d_ptr->volumeSlider, &QSlider::valueChanged, this, &PlayControlWidget::onVolumeSliderValueChanged);
    connect(d_ptr->rateBox, QOverload<int>::of(&QComboBox::activated), this, &PlayControlWidget::updateRate);
    connect(d_ptr->fullScreenBtn, &QToolButton::clicked, this, &PlayControlWidget::fullScreen);
}

void PlayControlWidget::updateDurationInfo(qint64 currentInfo)
{
    if (!currentInfo && !d_ptr->totalTime)
        return;
    QTime currentTime((currentInfo / 3600) % 60, (currentInfo / 60) % 60,
                      currentInfo % 60, (currentInfo * 1000) % 1000);
    QString format = "mm:ss";
    if (d_ptr->totalTime > 3600)
        format = "hh:mm:ss";
    d_ptr->durationLabel->setText(currentTime.toString(format));
}
