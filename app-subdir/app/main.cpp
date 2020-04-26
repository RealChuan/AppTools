#include <utils/logasync.h>
#include <utils/utils.h>
#include <extensionsystem/pluginmanager.h>
#include <extensionsystem/pluginspec.h>
#include <extensionsystem/iplugin.h>
#include <UserAccountSystem/loginwidget.h>

#include <QApplication>
#include <QDir>
#include <QThreadPool>
#include <QFont>
#include <QSettings>

using namespace ExtensionSystem;

int main(int argc, char *argv[])
{
    Utils::printBuildInfo();
    Utils::setHighDpiEnvironmentVariable();

    QApplication a(argc, argv);
    a.setApplicationVersion(QObject::tr("0.0.1"));
    a.setApplicationName(QObject::tr("AppPlugin"));
    a.setOrganizationName(QObject::tr("Youth"));

    const int threadCount = QThreadPool::globalInstance()->maxThreadCount();
    QThreadPool::globalInstance()->setMaxThreadCount(2 * threadCount);

    //qDebug() << threadCount;

    Utils::setUTF8Code();

    //异步日志
    //    LogAsync log;
    //    log.setLogLevel(QtDebugMsg);  //实际环境中可通过读取配置设置日志级别
    //    log.start();

    Utils::loadLanguage();
    Utils::setQSS();
    Utils::loadFonts();
    QDir::setCurrent(QCoreApplication::applicationDirPath());

    QSettings *setting = new QSettings(ConfigFile, QSettings::IniFormat);
    PluginManager pluginManager;
    PluginManager::setSettings(setting);
    PluginManager::setPluginIID(QLatin1String("Youth.Qt.plugin"));
    QStringList pluginPaths;
    pluginPaths << "./plugins" << "./test";
    PluginManager::setPluginPaths(pluginPaths);
    PluginManager::loadPlugins();

    // Shutdown plugin manager on the exit
    QObject::connect(&a, SIGNAL(aboutToQuit()), &pluginManager, SLOT(shutdown()));

    const QVector<PluginSpec *> plugins = PluginManager::plugins();
    PluginSpec *coreSpec = nullptr;
    for (PluginSpec *spec: plugins) {
        if (spec->name() == QLatin1String("CorePlugin")) {
            coreSpec = spec;
            break;
        }
    }

    if(coreSpec) {
        coreSpec->plugin()->remoteCommand(QStringList(), QString(), QStringList());
    } else {
        pluginManager.shutdown();
        a.quit();
        return -1;
    }

    return a.exec();
}
