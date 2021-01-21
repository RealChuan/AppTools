#include <utils/utils.h>
#include <utils/logasync.h>
#include <controls/waitwidget.h>
#include <crashhandler/crashhandler.h>
#include <extensionsystem/pluginmanager.h>
#include <extensionsystem/pluginspec.h>
#include <extensionsystem/iplugin.h>
#include <useraccountsystem/loginwidget.h>

#include <QApplication>
#include <QDir>
#include <QThreadPool>
#include <QFont>
#include <QSettings>
#include <QDebug>

using namespace ExtensionSystem;

int main(int argc, char *argv[])
{
    Utils::setHighDpiEnvironmentVariable();

    QApplication a(argc, argv);

    Utils::setCrashHandler();

    QDir::setCurrent(a.applicationDirPath());
    Utils::loadLanguage();

    // 异步日志
    Utils::LogAsync *log = Utils::LogAsync::instance();
    log->setOrientation(Utils::LogAsync::Orientation::StdAndFile);
    log->setLogLevel(QtDebugMsg);
    log->startWork();

    Utils::printBuildInfo();
    //Utils::setUTF8Code();
    Utils::loadFonts();
    Utils::setQSS();

    // 等待界面
    Control::WaitWidget waitWidget;
    waitWidget.show();
    a.processEvents();

    a.setApplicationVersion(QObject::tr("0.0.1"));
    a.setApplicationDisplayName(QObject::tr("AppPlugin"));
    a.setApplicationName(QObject::tr("AppPlugin"));
    a.setDesktopFileName(QObject::tr("AppPlugin"));
    a.setOrganizationDomain(QObject::tr("Youth"));
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
        coreSpec->plugin()->remoteCommand(QStringList(), QString(), QStringList());
        waitWidget.close();
    } else {
        pluginManager.shutdown();
        a.quit();
        return -1;
    }

    int result = a.exec();
    log->stop();
    return result;
}
