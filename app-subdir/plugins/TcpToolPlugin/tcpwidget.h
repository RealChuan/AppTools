#ifndef TCPWIDGET_H
#define TCPWIDGET_H

#include <QWidget>

#include "tcptoolplugin_global.h"

class TcpWidgetPrivate;
class  TCPTOOLPLUGIN_EXPORT TcpWidget : public QWidget
{
    Q_OBJECT
public:  
    enum MessageType { Send, Recv, SuccessInfo, ErrorInfo};

    explicit TcpWidget(QWidget *parent = nullptr);
    ~TcpWidget();

signals:

private slots:
    void onModelChange(const QString&);
    void onListenOrConnect(bool);
    void onSendData();

    void onServerOnline(bool);
    void onServerNewClient(const QString&);
    void onServerDisClient(const QString&);
    void onServerRecvMessage(const QString&, const QByteArray&);

    void onClientOnLine(bool);
    void onClientRecvMessage(const QByteArray&);
    void onAutoReconnectStartOrStop(bool);
    void onAutoConnect();

    void onAutoSend(bool);
    void onSave();
    void onAppendError(const QString&);

private:
    void setupUI();
    void init();
    void buildConnect();
    void clearCount();
    void appendDisplay(MessageType, const QString&);
    void setSendCount(int);
    void setRecvCount(int);

    void createTcpClientThread();
    void destoryServerOrClientThread();

    TcpWidgetPrivate *d;
};

#endif // TCPWIDGET_H
