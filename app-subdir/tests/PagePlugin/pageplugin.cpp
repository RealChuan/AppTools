#include "pageplugin.h"
#include "labelpage.h"
#include <extensionsystem/pluginmanager.h>

bool PagePlugin::initialize(const QStringList &, QString *)
{
    addObject(new LabelPage(this));
    return true;
}
