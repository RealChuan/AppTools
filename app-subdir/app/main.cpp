#include <utils/logasync.h>
#include <utils/utils.h>
#include <extensionsystem/pluginmanager.h>
#include <extensionsystem/pluginspec.h>
#include <extensionsystem/iplugin.h>
#include <useraccountsystem/loginwidget.h>
#include <controls/waitwidget.h>

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
    QDir::setCurrent(a.applicationDirPath());

    // 异步日志
    //LogAsync *log = LogAsync::instance();
    //log->setLogLevel(QtDebugMsg); // 实际环境中可通过读取配置设置日志级别
    //log->startWork();

    Utils::setUTF8Code();
    Utils::loadLanguage();
    Utils::loadFonts();
    Utils::setQSS();

    // 等待界面
    WaitWidget waitWidget;
    waitWidget.show();
    a.processEvents();

    a.setApplicationVersion(QObject::tr("0.0.1"));
    a.setApplicationName(QObject::tr("AppPlugin"));
    a.setOrganizationName(QObject::tr("Youth"));

    const int threadCount = QThreadPool::globalInstance()->maxThreadCount();
    QThreadPool::globalInstance()->setMaxThreadCount(2 * threadCount);

    //qDebug() << threadCount;

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
        waitWidget.fullProgressBar();
        coreSpec->plugin()->remoteCommand(QStringList(),
                                          QString(),
                                          QStringList());
        waitWidget.close();
    } else {
        pluginManager.shutdown();
        a.quit();
        return -1;
    }

    return a.exec();
}
