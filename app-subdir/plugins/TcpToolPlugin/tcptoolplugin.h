#ifndef TCPTOOLPLUGIN_H
#define TCPTOOLPLUGIN_H

#include <extensionsystem/iplugin.h>

class TcpToolPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "Youth.Qt.plugin" FILE "tcptoolplugin.json")
public:
    TcpToolPlugin() {}

    bool initialize(const QStringList &arguments, QString *errorString) override;
    void extensionsInitialized() override {}
};

#endif // TCPTOOLPLUGIN_H
