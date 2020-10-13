#include "playerwidget.h"
#include "mediawidget.h"

#include <QtWidgets>
#include <QMediaPlayer>
#include <QMediaPlaylist>

class PlayerWidgetPrivate{
public:
    PlayerWidgetPrivate(QWidget *parent)
        : owner(parent){
        mediaWidget = new MediaWidget(owner);

    }
    QWidget *owner;

    MediaWidget *mediaWidget;
};

PlayerWidget::PlayerWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new PlayerWidgetPrivate(this))
{
    setupUI();
}

PlayerWidget::~PlayerWidget()
{

}

void PlayerWidget::setupUI()
{

}
