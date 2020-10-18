#include "playcontrolwidget.h"

#include <QtWidgets>

class PlayControlWidgetPrivate{
public:
    PlayControlWidgetPrivate(QWidget *parent)
        : owner(parent){
        progressSlider = new QSlider(Qt::Horizontal, owner);
        durationLabel = new QLabel(owner);
        totalTimeLabel = new QLabel(owner);

        playButton = new QPushButton(owner);
        playButton->setCheckable(true);
        playButton->setEnabled(true);
        playButton->setObjectName("PlayButton");
        playButton->setToolTip(QObject::tr("Play|Pause"));

        previousButton = new QPushButton(owner);
        previousButton->setEnabled(true);
        previousButton->setObjectName("PreviousButton");
        previousButton->setToolTip(QObject::tr("Previous"));

        nextButton = new QPushButton(owner);
        nextButton->setEnabled(true);
        nextButton->setObjectName("NextButton");
        nextButton->setToolTip(QObject::tr("Next"));

        muteButton = new QPushButton(owner);
        muteButton->setCheckable(true);
        muteButton->setObjectName("MuteButton");
        muteButton->setToolTip(QObject::tr("Mute"));

        volumeSlider = new QSlider(Qt::Horizontal, owner);
        volumeSlider->setRange(0, 100);

        rateBox = new QComboBox(owner);
        rateBox->addItem("0.5x", QVariant(0.5));
        rateBox->addItem("1.0x", QVariant(1.0));
        rateBox->addItem("2.0x", QVariant(2.0));
        rateBox->setCurrentIndex(1);
    }
    QWidget *owner;
    QSlider *progressSlider;
    QLabel *durationLabel;
    QLabel *totalTimeLabel;
    QPushButton *playButton;
    QPushButton *previousButton;
    QPushButton *nextButton;
    QPushButton *muteButton;
    QSlider *volumeSlider;
    QComboBox *rateBox;

    qint64 totalTime;
    QMediaPlayer::State playerState = QMediaPlayer::StoppedState;
    int volume = 0;
};

PlayControlWidget::PlayControlWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new PlayControlWidgetPrivate(this))
{
    setupUI();
    buildConnect();
}

void PlayControlWidget::setPlayButtonEnable(bool enable)
{
    d_ptr->playButton->setEnabled(enable);
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
    return d_ptr->muteButton->isChecked();
}

qreal PlayControlWidget::playbackRate() const
{
    return d_ptr->rateBox->itemData(d_ptr->rateBox->currentIndex()).toDouble();
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
        break;
    case QMediaPlayer::PlayingState:
        d_ptr->playButton->setChecked(true);
        break;
    case QMediaPlayer::PausedState:
        d_ptr->playButton->setChecked(false);
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
    if (muted == d_ptr->muteButton->isChecked())
        return;
    d_ptr->muteButton->setChecked(muted);
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

void PlayControlWidget::onPlay()
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

void PlayControlWidget::onMuted(bool muted)
{
    emit changeMuting(muted);
    if(muted){
        d_ptr->volume = volume();
        setVolume(0);
    }else{
        setVolume(d_ptr->volume);
    }
}

void PlayControlWidget::updateRate()
{
    emit changeRate(playbackRate());
}

void PlayControlWidget::onVolumeSliderValueChanged()
{
    int v = volume();
    if(v == 0)
        setMuted(true);
    else
       setMuted(false);
    emit changeVolume(v);
}

void PlayControlWidget::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(event);
}

void PlayControlWidget::setupUI()
{
    QPushButton *setupButton = new QPushButton(this);
    setupButton->setObjectName("SetupButton");
    setupButton->setToolTip(tr("Set up"));
    //connect(setupButton, &QPushButton::clicked, this, );

    QPushButton *showListButton = new QPushButton(this);
    showListButton->setObjectName("ShowListButton");
    showListButton->setToolTip(tr("Show List|Hide List"));
    connect(showListButton, &QPushButton::clicked, this, &PlayControlWidget::showList);

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
    controlLayout->addWidget(setupButton);
    controlLayout->addWidget(showListButton);

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
    connect(d_ptr->previousButton, &QPushButton::clicked, this, &PlayControlWidget::previous);
    connect(d_ptr->nextButton, &QPushButton::clicked, this, &PlayControlWidget::next);
    connect(d_ptr->playButton, &QPushButton::clicked, this, &PlayControlWidget::onPlay);
    connect(d_ptr->muteButton, &QPushButton::clicked, this, &PlayControlWidget::onMuted);
    connect(d_ptr->volumeSlider, &QSlider::valueChanged, this, &PlayControlWidget::onVolumeSliderValueChanged);
    connect(d_ptr->rateBox, QOverload<int>::of(&QComboBox::activated), this, &PlayControlWidget::updateRate);
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
