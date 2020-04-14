#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QTcpSocket>

class TcpClientPrivate;
class TcpClient : public QTcpSocket
{
    Q_OBJECT
public:
    TcpClient(const QString &ip, quint16 port, QObject *parent = nullptr);
    ~TcpClient();

public slots:
    void oConnectToServer();
    void onWrite(const QByteArray&);

signals:
    void clientOnLine(bool);
    void errorMessage(const QString&);
    void serverMessage(const QByteArray&);

private slots:
    void onError();
    void onReadyRead();
    //void onStateChange(QAbstractSocket::SocketState socketState);
    void onDisconenct();

private:
    void buildConnect();

    TcpClientPrivate *d;
};

#endif // TCPCLIENT_H
