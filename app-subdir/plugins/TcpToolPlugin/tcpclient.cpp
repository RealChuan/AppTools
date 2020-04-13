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
}

void TcpClient::onError(QAbstractSocket::SocketError)
{
    QString error = tr("Client Error: %1").arg(errorString());
    emit errorMessage(error);
}

void TcpClient::onReadyRead()
{
    emit serverMessage(readAll());
}

void TcpClient::onStateChange(QAbstractSocket::SocketState socketState)
{
    bool state = (socketState == QAbstractSocket::ConnectedState);
    //emit clientOnLine(state);
}

void TcpClient::oConnectToServer()
{
    if(state() == QAbstractSocket::ConnectedState) return;

    connectToHost(d->ip, d->port);
    bool ok = waitForConnected();
    emit clientOnLine(ok);
}

void TcpClient::onWrite(const QByteArray &bytes)
{
    if(state() != QAbstractSocket::ConnectedState) return;

    write(bytes);
    waitForBytesWritten(200);
}

void TcpClient::buildConnect()
{
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));
    connect(this, &TcpClient::readyRead, this, &TcpClient::onReadyRead);
    connect(this, &TcpClient::stateChanged, this, &TcpClient::onStateChange);

    //connect(this, &TcpClient::clientOnLine, [](bool state) { qDebug() << "11111" << state;} );
}
