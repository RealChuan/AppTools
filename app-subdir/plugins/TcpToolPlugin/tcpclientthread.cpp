#include "tcpclientthread.h"
#include "tcpclient.h"

#include <QTcpSocket>

class TcpClientThreadPrivate{
public:
    TcpClientThreadPrivate(QThread *parent) : owner(parent){}

    QThread *owner;
    QString ip;
    quint16 port;
    bool autoReconnectFlag = false;
};

TcpClientThread::TcpClientThread(const QString &ip, quint16 port, bool flag, QObject *parent)
    : QThread(parent)
    , d(new TcpClientThreadPrivate(this))
{
    d->ip = ip;
    d->port = port;
    d->autoReconnectFlag = flag;
}

TcpClientThread::~TcpClientThread()
{
    join();
    delete  d;
}

void TcpClientThread::setAutoReconnect(bool state)
{
    d->autoReconnectFlag = state;
}

void TcpClientThread::run()
{
    QScopedPointer<TcpClient> tcpClient(new TcpClient(d->ip, d->port));
    connect(tcpClient.data(), &TcpClient::clientOnLine, this, &TcpClientThread::onClientOnLine);
    connect(tcpClient.data(), &TcpClient::errorMessage, this, &TcpClientThread::errorMessage);
    connect(tcpClient.data(), &TcpClient::serverMessage, this, &TcpClientThread::serverMessage);
    connect(this, &TcpClientThread::sendMessage, tcpClient.data(), &TcpClient::onWrite);
    connect(this, &TcpClientThread::reconnect, tcpClient.data(), &TcpClient::oConnectToServer);
    tcpClient->oConnectToServer();
    exec();
}

void TcpClientThread::onClientOnLine(bool state)
{
    emit clientOnLine(state);
    if(!state && !d->autoReconnectFlag)
        join();
}

void TcpClientThread::join()
{
    if(isRunning()){
        quit();
        wait();
    }
}
