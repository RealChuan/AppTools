#include "serialpage.h"
#include "serialwidget.h"

SerialPage::SerialPage(QObject *parent)
    : MPages(parent)
{
    init();
}

void SerialPage::init()
{
    setWidget(new SerialWidget);
    setButtonName(tr("Serial Assistant"));
    setButtonGroup(MPages::Tool);
}
