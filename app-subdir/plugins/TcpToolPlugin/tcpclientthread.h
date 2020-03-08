#ifndef TCPCLIENTTHREAD_H
#define TCPCLIENTTHREAD_H

#include <QAbstractSocket>
#include <QThread>

class TcpClientThread : public QThread
{
    Q_OBJECT
public:
    TcpClientThread(QObject *parent = nullptr);
    ~TcpClientThread() override;

signals:
    void clientOnLine(bool);
    void errorMessage(const QString&);
    void serverMessage(const QByteArray&);

protected:
    void run() override;

private slots:
    void onError(QAbstractSocket::SocketError);
    void onReadyRead();
};

#endif // TCPCLIENTTHREAD_H
