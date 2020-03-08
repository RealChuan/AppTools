#include "mpages.h"

#include <QtWidgets>

class MPagesPrivate{
public:
    MPagesPrivate(QObject *parent) : owner(parent){
        button = new QPushButton;
        //button->setFlat(true);
        button->setProperty("class", "GroupItemButton");
    }
    QObject *owner;
    QPushButton *button;
    QWidget *widget;
};

MPages::MPages(QObject *parent)
    : QObject(parent)
    , d(new MPagesPrivate(this))
{
}

MPages::~MPages()
{
    if(d->button){
        delete d->button;
        d->button = nullptr;
    }
    if(d->widget){
        delete d->widget;
        d->widget = nullptr;
    }
    delete d;
}

QPushButton *MPages::button() const
{
    return d->button;
}

QWidget *MPages::widget() const
{
    return d->widget;
}

void MPages::setWidget(QWidget *widget)
{
    d->widget = widget;
}

void MPages::setButtonName(const QString &text)
{
    d->button->setText(text);
}

void MPages::setButtonGroup(MPages::Group group)
{
    d->button->setProperty("Group", group);
}
