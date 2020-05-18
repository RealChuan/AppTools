#ifndef PAGEPLUGIN_H
#define PAGEPLUGIN_H

#include <extensionsystem/iplugin.h>

class PagePlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "Youth.Qt.plugin" FILE "pageplugin.json")
public:
    PagePlugin() {}

    bool initialize(const QStringList &arguments, QString *errorString) override;
    void extensionsInitialized() override {}
};

#endif // PAGEPLUGIN_H
