#include "crctoolplugin.h"
#include "crcpage.h"

bool CRCToolPlugin::initialize(const QStringList &, QString *)
{
    addObject(new CRCPage(this));
    return true;
}
