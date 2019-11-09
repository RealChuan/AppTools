#ifndef TCPWIDGET_H
#define TCPWIDGET_H

#include <QMainWindow>
#include <QtNetwork/QTcpServer>

namespace Ui {
class TcpWidget;
}

class TcpWidgetPrivate;

class TcpWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TcpWidget(QWidget *parent = nullptr);
    ~TcpWidget();

private slots:
    void initWindow();
    void choose();
    void initData();
    void builtConnect();
    void change(bool b);
    void openTcp();
    void append(quint8 type, QString msg);
    void sendData();
    void on_autosend_stateChanged(int arg1);
    void autoSendRestart();

    void newConnection();       //sever
    void disconnected();
    void serverRead();

    void readError(QAbstractSocket::SocketError);//client
    void clientRead();
    void autCconnectRestart();
    void connectRestart();
    void on_autoconnect_stateChanged(int arg1);
    void on_sendcount_clicked();
    void on_recvcount_clicked();
    //void on_savedata_clicked();
    void on_cleardata_clicked();

    void saveConfig();

private:
    Ui::TcpWidget *ui;
    TcpWidgetPrivate *d;
};

#endif // TCPWIDGET_H
