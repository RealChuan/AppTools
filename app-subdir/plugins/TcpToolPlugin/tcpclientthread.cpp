#include "tcpclientthread.h"

#include <QTcpSocket>

TcpClientThread::TcpClientThread(QObject *parent) : QThread(parent)
{

}

TcpClientThread::~TcpClientThread()
{
    if(isRunning()){
        quit();
        wait();
    }
}

void TcpClientThread::run()
{
    QScopedPointer<QTcpSocket> tcpSocket(new QTcpSocket);
    connect(tcpSocket.data(), SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(onError(QAbstractSocket::SocketError)), Qt::DirectConnection);
    connect(tcpSocket.data(), &QTcpSocket::readyRead,
            this, &TcpClientThread::onReadyRead, Qt::DirectConnection);
}

void TcpClientThread::onError(QAbstractSocket::SocketError)
{
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if(nullptr == client) return;

    QString error = tr("Client Error: %1").arg(client->errorString());
    emit errorMessage(error);
}

void TcpClientThread::onReadyRead()
{
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if(nullptr == client) return;

    QByteArray bytes = client->readAll();
    emit serverMessage(bytes);
}
