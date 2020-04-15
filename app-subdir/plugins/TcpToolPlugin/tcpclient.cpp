#include "tcpclient.h"

class TcpClientPrivate{
public:
    TcpClientPrivate(QTcpSocket *parent) : owner(parent){}
    QTcpSocket *owner;
    QString ip;
    quint16 port;
};

TcpClient::TcpClient(const QString &ip, quint16 port, QObject *parent)
    : QTcpSocket(parent)
    , d(new TcpClientPrivate(this))
{
    d->ip = ip;
    d->port = port;
    buildConnect();
}

TcpClient::~TcpClient()
{
    delete d;
    if(isOpen())
        close();
}

void TcpClient::oConnectToServer()
{
    if(state() == QAbstractSocket::ConnectedState) return;

    connectToHost(d->ip, d->port);
    bool ok = waitForConnected(1000);
    emit clientOnLine(ok);

    if(!ok) onError();
}

void TcpClient::onWrite(const QByteArray &bytes)
{
    if(state() != QAbstractSocket::ConnectedState) return;

    write(bytes);
    waitForBytesWritten(100);
}

void TcpClient::onError()
{
    QString error = tr("Client Error: %1").arg(errorString());
    emit errorMessage(error);
}

void TcpClient::onReadyRead()
{
    if(bytesAvailable() <= 0 ) return;

    QByteArray bytes;
    while (!atEnd())
        bytes += readAll();

    emit serverMessage(bytes);
}

//void TcpClient::onStateChange(QAbstractSocket::SocketState socketState)
//{
//    bool state = (socketState == QAbstractSocket::ConnectedState);
//    emit clientOnLine(state);
//}

void TcpClient::onDisconenct()
{
    emit clientOnLine(false);
}

void TcpClient::buildConnect()
{
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError()));
    connect(this, &TcpClient::readyRead, this, &TcpClient::onReadyRead);
    //connect(this, &TcpClient::stateChanged, this, &TcpClient::onStateChange);
    connect(this, &TcpClient::disconnected, this, &TcpClient::onDisconenct);
}
