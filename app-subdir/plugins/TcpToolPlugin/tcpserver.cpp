#include "tcpserver.h"

#include <QTcpSocket>

class TcpServerPrivate{
public:
    TcpServerPrivate(QTcpServer *parent) : owner(parent){}
    QTcpServer *owner;
    QVector<QTcpSocket*> tcpClientList;
};

TcpServer::TcpServer(QObject *parent) : QTcpServer(parent)
  , d(new TcpServerPrivate(this))
{
    buildConnect();
}

TcpServer::~TcpServer()
{
    delete d;
    if(isListening())
        close();
}

void TcpServer::onSendMessage(const QByteArray &bytes, const QString &clientInfo)
{
    if(bytes.isEmpty()) return;

    if(clientInfo.isEmpty()) {
        for(QTcpSocket* client : d->tcpClientList)
            client->write(bytes);
    } else {
        QString clientIP = clientInfo.split(":")[0].trimmed();
        int clientPort = clientInfo.split(":")[1].toInt();
        for(QTcpSocket* client : d->tcpClientList){
            if(client->peerAddress().toString().split("::ffff:")[0] == clientIP &&
                    client->peerPort() == clientPort)
                client->write(bytes);
        }
    }
}

void TcpServer::onError()
{
    QString error = tr("TCPServer accept Error: %1").arg(errorString());
    emit errorMessage(error);
}

void TcpServer::onNewConnect()
{
    QTcpSocket *client = nextPendingConnection();
    client->setParent(this);    //利用Qt的对象树进行析构
    d->tcpClientList.push_back(client);

    QString clientInfo = tr("%1 : %2").
            arg(client->peerAddress().toString().split("::ffff:")[0]).
            arg(client->peerPort());
    emit newClientInfo(clientInfo);

    connect(client, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(onClientError(QAbstractSocket::SocketError)));
    connect(client, &QTcpSocket::disconnected, this, &TcpServer::onClientDisconnect);
    connect(client, &QTcpSocket::readyRead, this, &TcpServer::onClientReadyRead);
}

void TcpServer::onClientError(QAbstractSocket::SocketError)
{
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if(nullptr == client) return;

    QString error = tr("Client [%1 : %2] Error: %3.").
            arg(client->peerAddress().toString().split("::ffff:")[0]).
            arg(client->peerPort()).arg(client->errorString());

    emit errorMessage(error);
}

void TcpServer::onClientDisconnect()
{
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if(nullptr == client) return;

    if(client->state() == QAbstractSocket::UnconnectedState){
        QString clientInfo = tr("%1 : %2").
                arg(client->peerAddress().toString().split("::ffff:")[0]).
                arg(client->peerPort());
        emit disconnectClientInfo(clientInfo);
        d->tcpClientList.removeOne(client);
        client->deleteLater();
        client = nullptr;
    }
}

void TcpServer::onClientReadyRead()
{
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if(nullptr == client) return;

    if(client->bytesAvailable() <= 0) return;

    QByteArray bytes;
    while (client->atEnd())
        bytes += client->readAll();
    //if(bytes.isEmpty()) return;

    QString clientInfo = tr("Client [%1 : %2] : ").
            arg(client->peerAddress().toString().split("::ffff:")[0]).
            arg(client->peerPort());
    emit clientMessage(clientInfo, bytes);
}

void TcpServer::buildConnect()
{
    connect(this, &TcpServer::acceptError, this, &TcpServer::onError);
    connect(this, &TcpServer::newConnection, this, &TcpServer::onNewConnect);
}
