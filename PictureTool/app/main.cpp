#include <utils/logasync.h>
#include <utils/utils.h>
#include <controls/waitwidget.h>
#include <mainwindow/mainwindow.h>

#include <QApplication>
#include <QDir>
#include <QThreadPool>
#include <QFont>
#include <QSettings>

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
    a.setApplicationDisplayName(QObject::tr("PictureTool"));
    a.setApplicationName(QObject::tr("PictureTool"));
    a.setOrganizationName(QObject::tr("Youth"));

    const int threadCount = QThreadPool::globalInstance()->maxThreadCount();
    QThreadPool::globalInstance()->setMaxThreadCount(2 * threadCount);

    //qDebug() << threadCount;

    MainWindow w;
    w.show();
    waitWidget.close();

    return a.exec();
}
