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
    void sendData();

    void onServerOnline(bool);
    void onServerNewClient(const QString&);
    void onServerDisClient(const QString&);
    void onServerRecvMessage(const QString&, const QByteArray&);

    void onAutoSend(bool);
    void onSave();
    void appendError(const QString&);

private:
    void setupUI();
    void init();
    void buildConnect();
    void clearButton();
    void appendDisplay(MessageType, const QString&);
    void sendCount(int);
    void recvCount(int);

    TcpWidgetPrivate *d;
};

#endif // TCPWIDGET_H
