#include "crcpage.h"
#include "crcwidget.h"

CRCPage::CRCPage(QObject *parent)
    : MPages(parent)
{
    init();
}

void CRCPage::init()
{
    setWidget(new CRCWidget);
    setButtonName(tr("CRC Assistant"));
    setButtonGroup(MPages::Tool);
    setButtonObejctName("CrcButton");
}
