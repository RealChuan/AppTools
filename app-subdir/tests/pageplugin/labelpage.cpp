#include "labelpage.h"
#include "labelframe.h"

LabelPage::LabelPage(QObject *parent)
    : MPages(parent)
{
    init();
}

void LabelPage::init()
{
    setWidget(new LabelFrame);
    setButtonName(tr("Page Test"));
    setButtonGroup(MPages::Test);
    setButtonObejctName("PageTestButton");
}
