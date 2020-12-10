#include "tcppage.h"
#include "tcpwidget.h"

TcpPage::TcpPage(QObject *parent)
    : MPages(parent)
{
    init();
}

void TcpPage::init()
{
    setWidget(new TcpWidget);
    setButtonName(tr("Tcp Assistant"));
    setButtonGroup(MPages::Tool);
    setButtonObejctName("TcpButton");
}
