#include "utils.h"
#include "json.h"
#include "hostosinfo.h"

#include <QtWidgets>

void Utils::setUTF8Code()
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
}

void Utils::setQSS()
{
    Json json("./config/config.json", true);
    QStringList qssPath = json.getStringList("qss_files");
    QString qss;
    foreach(const QString& path, qssPath){
        qDebug() << QString(QObject::tr("Loading QSS file: %1.")).arg(path);
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << QString(QObject::tr("Cannot open the file: %1!")).arg(path);
            qDebug() << file.errorString();
            continue;
        }
        qss.append(QLatin1String(file.readAll())).append("\n");
        file.close();
    }
    if(!qss.isEmpty())
        qApp->setStyleSheet(qss);
}

void Utils::loadFonts()
{
    Json json("./config/config.json", true);
    QStringList fontFiles = json.getStringList("font_files");

    for (const QString &file : fontFiles) {
        qDebug() << QString(QObject::tr("Loading Font file: %1")).arg(file);
        QFontDatabase::addApplicationFont(file);
    }
}

void Utils::windowCenter(QWidget *window)
{
    QSize size = qApp->primaryScreen()->availableSize() - window->size();
    int x = qMax(0, size.width() / 2);
    int y = qMax(0, size.height() / 2);
    window->move(x, y);
}

void Utils::sleep(int sec)
{
    QTime dieTime = QTime::currentTime().addMSecs(sec);

    while (QTime::currentTime() < dieTime) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}

bool Utils::checkFileExist(const QString &path)
{
    QFile file(path);
    if(file.size() == 0) return false;
    return true;
}

QString compilerString()
{
#if defined(Q_CC_CLANG) // must be before GNU, because clang claims to be GNU too
    QString isAppleString;
#endif
#if defined(__apple_build_version__) // Apple clang has other version numbers
    isAppleString = QLatin1String(" (Apple)");
    return QLatin1String("Clang " ) + QString::number(__clang_major__) + QLatin1Char('.')
            + QString::number(__clang_minor__) + isAppleString;
#elif defined(Q_CC_GNU)
    return QLatin1String("GCC " ) + QLatin1String(__VERSION__);
#elif defined(Q_CC_MSVC)
    if (_MSC_VER > 1999)
        return QLatin1String("MSVC <unknown>");
    if (_MSC_VER >= 1910)
        return QLatin1String("MSVC 2017");
    if (_MSC_VER >= 1900)
        return QLatin1String("MSVC 2015");
    if (_MSC_VER >= 1800)
        return QLatin1String("MSVC 2013");
#endif
    return QLatin1String("<unknown compiler>");
}

void Utils::printBuildInfo()
{
    QString info = QString("Qt %1 (%2, %3 bit)").
            arg(QLatin1String(qVersion()), compilerString(),
                QString::number(QSysInfo::WordSize));
    qDebug().noquote() << QObject::tr("Build with:") << info;

}
void Utils::setHighDpiEnvironmentVariable()
{
    if (Utils::HostOsInfo().isMacHost())
        return;

    //#ifdef Q_OS_WIN
    //    if (!qEnvironmentVariableIsSet("QT_OPENGL"))
    //        QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
    //#endi

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    // work around QTBUG-80934
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
                Qt::HighDpiScaleFactorRoundingPolicy::Round);
#endif
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

}
