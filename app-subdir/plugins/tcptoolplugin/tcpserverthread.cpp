#include "tcpserverthread.h"
#include "tcpserver.h"

class TcpServerThreadPrivate{
public:
    TcpServerThreadPrivate(QThread *parent) : owner(parent) {}
    QThread *owner;
    QString ip;
    quint16 port;
};

TcpServerThread::TcpServerThread(quint16 port, const QString &ip,
                                 QObject *parent)
    : QThread(parent)
    , d(new TcpServerThreadPrivate(this))
{
    d->ip = ip;
    d->port = port;
}

TcpServerThread::~TcpServerThread()
{
    if(isRunning()){
        quit();
        wait();
    }
}

void TcpServerThread::run()
{
    QScopedPointer<TcpServer> tcpServer(new TcpServer);
    connect(tcpServer.data(), &TcpServer::errorMessage, this, &TcpServerThread::errorMessage);
    connect(tcpServer.data(), &TcpServer::newClientInfo, this, &TcpServerThread::newClientInfo);
    connect(tcpServer.data(), &TcpServer::disconnectClientInfo, this, &TcpServerThread::disconnectClientInfo);
    connect(tcpServer.data(), &TcpServer::clientMessage, this, &TcpServerThread::clientMessage);
    connect(this, &TcpServerThread::sendMessage, tcpServer.data(), &TcpServer::onSendMessage);

    bool ok = tcpServer->listen(QHostAddress(d->ip), d->port);
    emit serverOnLine(ok);
    if(!ok) return;

    exec();

    emit serverOnLine(false);
}
