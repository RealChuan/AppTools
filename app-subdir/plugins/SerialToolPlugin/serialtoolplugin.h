#ifndef SERIALTOOLPLUGIN_H
#define SERIALTOOLPLUGIN_H

#include <extensionsystem/iplugin.h>

class SerialToolPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "Youth.Qt.plugin" FILE "serialtoolplugin.json")
public:
    SerialToolPlugin() {}

    bool initialize(const QStringList &arguments, QString *errorString) override;
    void extensionsInitialized() override {}
};

#endif // SERIALTOOLPLUGIN_H
