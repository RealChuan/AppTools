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

    // 异步日志
    //LogAsync *log = LogAsync::instance();
    //log->setLogLevel(QtDebugMsg); // 实际环境中可通过读取配置设置日志级别
    //log->startWork();

    Utils::printBuildInfo();
    Utils::setUTF8Code();
    //Utils::loadFonts();
    //Utils::setQSS();

    const int threadCount = QThreadPool::globalInstance()->maxThreadCount();
    QThreadPool::globalInstance()->setMaxThreadCount(2 * threadCount);

    //qDebug() << threadCount;

    MainWindow w;
    w.show();

    int result = a.exec();
    //log->stop();
    return result;
}
