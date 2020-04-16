#ifndef CRCTOOLPLUGIN_H
#define CRCTOOLPLUGIN_H

#include <extensionsystem/iplugin.h>

class CRCToolPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "Youth.Qt.plugin" FILE "crctoolplugin.json")
public:
    CRCToolPlugin() {}

    bool initialize(const QStringList &arguments, QString *errorString) override;
    void extensionsInitialized() override {}
};

#endif // CRCTOOLPLUGIN_H
