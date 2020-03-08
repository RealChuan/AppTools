#include "utils.h"
#include "json.h"

#include <QtWidgets>

QString Utils::appPath()
{
    return qApp->applicationDirPath();
}

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
        int fontId = QFontDatabase::addApplicationFont(file);
        QString f = QFontDatabase::applicationFontFamilies(fontId).at(0);
        qApp->setFont(QFont(f, 10, QFont::Normal, false));
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
