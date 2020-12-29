#include <utils/logasync.h>
#include <utils/utils.h>
#include <mainwindow/mainwindow.h>

#include <QApplication>
#include <QDir>
#include <QThreadPool>

int main(int argc, char *argv[])
{
    Utils::setHighDpiEnvironmentVariable();

    QApplication a(argc, argv);
    QDir::setCurrent(a.applicationDirPath());
    Utils::loadLanguage();

    a.setApplicationVersion(QObject::tr("0.0.1"));
    a.setApplicationDisplayName(QObject::tr("Media Player"));
    a.setApplicationName(QObject::tr("Media Player"));
    a.setOrganizationName(QObject::tr("Youth"));

    // 异步日志
    Utils::LogAsync *log = Utils::LogAsync::instance();
    log->setOrientation(Utils::LogAsync::Orientation::StdAndFile);
    log->setLogLevel(QtDebugMsg);
    log->startWork();

    Utils::printBuildInfo();
    //Utils::setUTF8Code();
    Utils::loadFonts();
    Utils::setQSS();

    const int threadCount = QThreadPool::globalInstance()->maxThreadCount();
    QThreadPool::globalInstance()->setMaxThreadCount(2 * threadCount);
    //qDebug() << threadCount;

    MainWindow w;
    w.show();

    int result = a.exec();
    log->stop();
    return result;
}
