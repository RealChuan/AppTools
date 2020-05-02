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

    // 异步日志
    //    LogAsync log;
    //    log.setLogLevel(QtDebugMsg);  //实际环境中可通过读取配置设置日志级别
    //    log.start();

    Utils::loadLanguage();
    Utils::setUTF8Code();
    Utils::setQSS();

    // 等待界面
    WaitWidget waitWidget;
    waitWidget.show();
    a.processEvents();

    a.setApplicationVersion(QObject::tr("0.0.1"));
    a.setApplicationName(QObject::tr("PictureTool"));
    a.setOrganizationName(QObject::tr("Youth"));

    const int threadCount = QThreadPool::globalInstance()->maxThreadCount();
    QThreadPool::globalInstance()->setMaxThreadCount(2 * threadCount);

    //qDebug() << threadCount;

    Utils::loadFonts();
    QDir::setCurrent(QCoreApplication::applicationDirPath());

    MainWindow w;
    w.show();
    waitWidget.close();

    return a.exec();
}
