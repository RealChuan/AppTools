#include <utils/hostosinfo.h>
#include <utils/utils.h>
#include <extensionsystem/pluginmanager.h>
#include <extensionsystem/pluginspec.h>
#include <extensionsystem/iplugin.h>

#include <QApplication>
#include <QDir>
#include <QThreadPool>
#include <QFont>
#include <QSettings>

using namespace ExtensionSystem;

static QSettings *createUserSettings()
{
    return new QSettings(QLatin1String("./config/config.ini", QSettings::IniFormat));
}

static void setHighDpiEnvironmentVariable()
{

    if (Utils::HostOsInfo().isMacHost())
        return;

    std::unique_ptr<QSettings> settings(createUserSettings());

    const bool defaultValue = Utils::HostOsInfo().isWindowsHost();
    const bool enableHighDpiScaling = settings->value("Core/EnableHighDpiScaling", defaultValue).toBool();

    static const char ENV_VAR_QT_DEVICE_PIXEL_RATIO[] = "QT_DEVICE_PIXEL_RATIO";
    if (enableHighDpiScaling
            && !qEnvironmentVariableIsSet(ENV_VAR_QT_DEVICE_PIXEL_RATIO) // legacy in 5.6, but still functional
            && !qEnvironmentVariableIsSet("QT_AUTO_SCREEN_SCALE_FACTOR")
            && !qEnvironmentVariableIsSet("QT_SCALE_FACTOR")
            && !qEnvironmentVariableIsSet("QT_SCREEN_SCALE_FACTORS")) {
        QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
        // work around QTBUG-80934
        QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
                    Qt::HighDpiScaleFactorRoundingPolicy::Round);
#endif
    }
}

int main(int argc, char *argv[])
{
#ifdef Q_OS_WIN
    if (!qEnvironmentVariableIsSet("QT_OPENGL"))
        QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
#endif

    setHighDpiEnvironmentVariable();

    QApplication a(argc, argv);
    a.setAttribute(Qt::AA_UseHighDpiPixmaps);
    //a.setFont(QFont ("Microsoft YaHei", 10, QFont::Normal, false));
    a.setApplicationVersion(QObject::tr("0.0.1"));
    a.setApplicationName(QObject::tr("App"));
    a.setOrganizationName(QObject::tr("Youth"));

    const int threadCount = QThreadPool::globalInstance()->maxThreadCount();
    QThreadPool::globalInstance()->setMaxThreadCount(2 * threadCount);

    Utils::setUTF8Code();
    Utils::setQSS();
    Utils::loadFonts();
    QDir::setCurrent(QCoreApplication::applicationDirPath());

    PluginManager pluginManager;
    PluginManager::setPluginIID(QLatin1String("Youth.Qt.plugin"));
    QStringList pluginPaths;
    pluginPaths << "./plugins" << "./test";
    PluginManager::setPluginPaths(pluginPaths);
    PluginManager::loadPlugins();

    // Shutdown plugin manager on the exit
    QObject::connect(&a, SIGNAL(aboutToQuit()), &pluginManager, SLOT(shutdown()));

    const QVector<PluginSpec *> plugins = PluginManager::plugins();
    PluginSpec *coreSpec = nullptr;
    foreach (PluginSpec *spec, plugins) {
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
