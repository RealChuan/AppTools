#include "tcptoolplugin.h"
#include "tcppage.h"

bool TcpToolPlugin::initialize(const QStringList &, QString *)
{
    addObject(new TcpPage(this));
    return true;
}
