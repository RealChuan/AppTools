#include <utils/logasync.h>
#include <utils/utils.h>
#include <controls/waitwidget.h>
#include <crashhandler/crashhandler.h>
#include <mainwindow/mainwindow.h>

#include <QApplication>
#include <QDir>
#include <QThreadPool>

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
    WaitWidget waitWidget;
    waitWidget.show();
    a.processEvents();

    a.setApplicationVersion(QObject::tr("0.0.1"));
    a.setApplicationDisplayName(QObject::tr("PictureTool"));
    a.setApplicationName(QObject::tr("PictureTool"));
    a.setOrganizationName(QObject::tr("Youth"));

    const int threadCount = QThreadPool::globalInstance()->maxThreadCount();
    QThreadPool::globalInstance()->setMaxThreadCount(2 * threadCount);

    //qDebug() << threadCount;

    MainWindow w;
    w.show();
    waitWidget.close();

    int result = a.exec();
    log->stop();
    return result;
}
