#include "serialpage.h"
#include "serialtoolplugin.h"

bool SerialToolPlugin::initialize(const QStringList &, QString *)
{
    addObject(new SerialPage(this));
    return true;
}
