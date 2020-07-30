#include "tcpclientthread.h"
#include "tcpclient.h"

#include <QTcpSocket>

struct TcpClientThreadPrivate{
    QString ip;
    quint16 port;
};

TcpClientThread::TcpClientThread(const QString &ip, quint16 port, QObject *parent)
    : QThread(parent)
    , d(new TcpClientThreadPrivate)
{
    d->ip = ip;
    d->port = port;
}

TcpClientThread::~TcpClientThread()
{
    if(isRunning()){
        quit();
        wait();
    }
    qDebug() << "~TcpClientThread";
}

void TcpClientThread::run()
{
    QScopedPointer<TcpClient> tcpClient(new TcpClient(d->ip, d->port));
    connect(tcpClient.data(), &TcpClient::clientOnLine, this, &TcpClientThread::clientOnLine);
    connect(tcpClient.data(), &TcpClient::errorMessage, this, &TcpClientThread::errorMessage);
    connect(tcpClient.data(), &TcpClient::serverMessage, this, &TcpClientThread::serverMessage);
    connect(this, &TcpClientThread::sendMessage, tcpClient.data(), &TcpClient::onWrite);
    connect(this, &TcpClientThread::reconnect, tcpClient.data(), &TcpClient::oConnectToServer);
    tcpClient->oConnectToServer();
    exec();

    emit clientOnLine(false);
}
