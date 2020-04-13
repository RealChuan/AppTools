#ifndef TCPCLIENTTHREAD_H
#define TCPCLIENTTHREAD_H

#include <QAbstractSocket>
#include <QThread>

class TcpClientThreadPrivate;
class TcpClientThread : public QThread
{
    Q_OBJECT
public:
    TcpClientThread(const QString &ip, quint16 port, bool flag = false, QObject *parent = nullptr);
    ~TcpClientThread() override;

    void setAutoReconnect(bool);

signals:
    void clientOnLine(bool);
    void errorMessage(const QString&);
    void serverMessage(const QByteArray&);
    void sendMessage(const QByteArray&);

    void reconnect();

protected:
    void run() override;

private slots:
    void onClientOnLine(bool);

private:
    void join();

    TcpClientThreadPrivate *d;
};

#endif // TCPCLIENTTHREAD_H
