#include "crashdialog.h"

#include <QtWidgets>

namespace Utils {

CrashDialog::CrashDialog(QWidget *parent)
    : Dialog(parent)
{
    init();
    setupUI();
    resize(300, 400);
}

CrashDialog::~CrashDialog()
{

}

void CrashDialog::onOpenCrashPath()
{
    QDesktopServices::openUrl(QUrl(qApp->applicationDirPath() + "/crashes", QUrl::TolerantMode));
}

void CrashDialog::onRestart()
{
    qApp->closeAllWindows();
    QProcess::startDetached(qApp->applicationFilePath(), QStringList());
}

void CrashDialog::onQuit()
{
    qApp->exit();
}

void CrashDialog::setupUI()
{
    QPushButton *crashButton = new QPushButton(tr("Path of Crash File"), (this));
    QPushButton *restartButton = new QPushButton(tr("Restart"), this);
    QPushButton *closeButton = new QPushButton(tr("Close"), this);
    crashButton->setObjectName("BlueButton");
    restartButton->setObjectName("BlueButton");
    closeButton->setObjectName("BlueButton");
    connect(crashButton, &QPushButton::clicked, this, &CrashDialog::onOpenCrashPath);
    connect(restartButton, &QPushButton::clicked, this, &CrashDialog::onRestart);
    connect(closeButton, &QPushButton::clicked, this, &CrashDialog::onQuit);

    QLabel *crashLabel = new QLabel(this);
    crashLabel->setObjectName("CrashLabel");
    crashLabel->setWordWrap(true);
    crashLabel->setText(tr("Sorry, the application crashed abnormally. \n"
                           "please click [Path of Crash File] \n"
                           "and send us all the files in this path. \n"
                           "This can help us fix bugs. \n"
                           "Thank you! \n"
                           "Contact Me - Email: \n"
                           "1070753498@qq.com"));

    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->addStretch();
    layout->addWidget(crashLabel);
    layout->addStretch();
    layout->addWidget(crashButton);
    layout->addWidget(restartButton);
    layout->addWidget(closeButton);
    setCentralWidget(widget);
}

void CrashDialog::init()
{
    QString strTime = QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss-zzz");
    QString path = QString("%1/crashes/%2-System Environment.txt")
            .arg(qApp->applicationDirPath())
            .arg(strTime);

    QFile file(path);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QStringList systemEnviroment = QProcess::systemEnvironment();
        QString str;
        for(const QString& info: systemEnviroment){
            str += info;
            str += '\n';
        }
        file.write(str.toUtf8());
        file.flush();
        file.close();
    }else{
        qDebug() << file.errorString();
    }
}

}
