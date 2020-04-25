#include "coreplugin.h"
//#include "mainwindow.h"

#include <QApplication>

bool CorePlugin::initialize(const QStringList&, QString*)
{
    m_mainWindow.reset(new MainWindow);
    return true;
}

void CorePlugin::extensionsInitialized()
{
    m_mainWindow->extensionsInitialized();
}

QObject *CorePlugin::remoteCommand(const QStringList &,
                                   const QString &,
                                   const QStringList &)
{
    qApp->setApplicationDisplayName(QObject::tr("AppPlugin"));
    m_mainWindow->show();
    m_mainWindow->onAccount(true);
    return nullptr;
}
