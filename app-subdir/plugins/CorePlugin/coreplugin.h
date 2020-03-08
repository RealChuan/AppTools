#ifndef COREPLUGIN_H
#define COREPLUGIN_H

#include <extensionsystem/iplugin.h>

#include "mainwindow.h"

//class MainWindow;
class CorePlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "Youth.Qt.plugin" FILE "coreplugin.json")
public:
    CorePlugin() {}

    bool initialize(const QStringList &arguments, QString *errorString) override;
    void extensionsInitialized() override;
    QObject *remoteCommand(const QStringList &, const QString &, const QStringList &) override;

private:
    QScopedPointer<MainWindow> m_mainWindow;
};

#endif // COREPLUGIN_H
