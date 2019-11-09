﻿#include "TcpWidget.h"
#include "ui_TcpWidget.h"
#include "../help/BinaryCvn.h"
#include "../help/UiSet.h"
#include "../help/AppCfg.h"

#include <QNetworkInterface>
#include <QTcpSocket>

#pragma execution_character_set("utf-8")

class TcpWidgetPrivate{
public:
    TcpWidgetPrivate(QWidget *owner):
        owner(owner)
    {
        server = new QTcpServer(owner);
        client = new QTcpSocket(owner);   //实例化tcpClient
        sendTime = new QTimer(owner);
        timeConnect=new QTimer(owner);
    }
    QWidget *owner;
    QTcpServer *server;          //server
    QList<QTcpSocket*> serverClient;
    QTcpSocket *serverCurrentClient;

    QTcpSocket *client;          //client
    QTimer *timeConnect;

    bool ok;
    int recvCount;                  //接收数据计数
    int sendCount;                  //发送计数
    QTimer *sendTime;               //定时发送串口数据
};

TcpWidget::TcpWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TcpWidget),
    d(new TcpWidgetPrivate(this))
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);   //去掉边框
    setAttribute(Qt::WA_StyledBackground);
    initWindow();
    choose();
    builtConnect();
}

TcpWidget::~TcpWidget()
{
    saveConfig();
    delete ui;
    delete d;
}

void TcpWidget::initWindow()
{
    QStringList modellist;
    modellist<<"TCPServer"<<"TCPClient";
    ui->model->addItems(modellist);
    //ui->model->setCurrentIndex(0);

    //IP输入框正则
    QRegExp regExp("^((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)$");
    QValidator *validator = new QRegExpValidator(regExp, ui->ip);
    ui->ip->setValidator(validator);
    ui->port->setValidator(new QIntValidator(0, 65536,ui->port));

    //ui->port->setText("8000");

    QStringList sendInterval;//自动发送间隔
    for (int i = 100; i < 1000; i = i + 100)
        sendInterval << QString::number(i);
    for (int i = 1000; i <= 10000; i = i + 1000)
        sendInterval << QString::number(i);
    ui->autosendtime->addItems(sendInterval);
    //ui->autosendtime->setCurrentIndex(9);
    ui->autoconnecttime->addItems(sendInterval);
    //ui->autoconnecttime->setCurrentIndex(9);

    ui->allconnect->addItem("全部连接");

    //ui->hexsend->setChecked(true);
    //ui->hexrecv->setChecked(true);
    ui->autosend->setChecked(false);
    ui->autosend->setEnabled(false);
    ui->sendbtn->setEnabled(false);

    //ui->display->setFontPointSize(10);//设置字体大小
    QApplication::setFont(QFont ("微软雅黑", 10, QFont::Normal, false));

    AppCfg::readConfig();                          //读取配置
    ui->model->setCurrentIndex(ui->model->findText(AppCfg::tcpModel));
    ui->ip->setText(AppCfg::tcpIp);
    ui->port->setText(QString::number(AppCfg::tcpPort));
    ui->hexrecv->setChecked(AppCfg::tcpHexRecv);
    ui->hexsend->setChecked(AppCfg::tcpHexSend);
    ui->autosendtime->setCurrentIndex(ui->autosendtime->findText(QString::number(AppCfg::tcpAutoSendTime)));
    ui->autoconnecttime->setCurrentIndex(ui->autoconnecttime->findText(QString::number(AppCfg::tcpAutoSendTime)));
    ui->senddata->setText(AppCfg::tcpData);
    //qDebug()<<AppCfg::port;
    ui->listen->setCheckable(true);
}

void TcpWidget::initData()
{
    d->ok=false;
    d->recvCount = 0;
    d->sendCount = 0;
    ui->sendcount->setText("发送 : 0 字节");
    ui->recvcount->setText("接收 : 0 字节");

    ui->iplist->clear();
    QStringList iplistbox;
    QList<QHostAddress> iplist = QNetworkInterface::allAddresses(); //获得IP
    foreach(QHostAddress address,iplist)
    {
        if(address.protocol() == QAbstractSocket::IPv4Protocol)
        {
            iplistbox<<address.toString();
        }
    }
    ui->iplist->addItems(iplistbox);
    //ui->ip->setText(iplistbox[0]);
}

void TcpWidget::choose()
{
    initData();
    if(ui->model->currentIndex()==0)
    {
        ui->listen->setText("监听");
        ui->ip->hide();
        ui->iplist->show();
        ui->allconnect->show();
        ui->autoconnect->hide();
        ui->autoconnecttime->hide();
        ui->ip->setReadOnly(true);
        ui->iplabel->setText("本机IP列表:");
        ui->portlabel->setText("本机端口:");
    }
    else if(ui->model->currentIndex()==1)
    {
        ui->listen->setText("连接");
        ui->ip->show();
        ui->iplist->hide();
        ui->allconnect->hide();
        ui->autoconnect->show();
        ui->autoconnecttime->show();
        ui->ip->setReadOnly(false);
        ui->iplabel->setText("服务器IP:");
        ui->portlabel->setText("服务器端口:");
        d->client->abort();                 //取消原有连接
    }
}

void TcpWidget::builtConnect()
{    //
    connect(ui->model, SIGNAL(currentIndexChanged(int)), this, SLOT(choose()));
    connect(ui->listen,SIGNAL(clicked()),this,SLOT(openTcp()));

    connect(d->server, SIGNAL(newConnection()), this, SLOT(newConnection()));

    d->sendTime->setInterval(ui->autosendtime->currentText().toInt());
    connect(ui->autosendtime, SIGNAL(currentIndexChanged(int)), this, SLOT(autoSendRestart()));
    //发送数据
    connect(d->sendTime, SIGNAL(timeout()), this, SLOT(sendData()));
    connect(ui->sendbtn, SIGNAL(clicked()), this, SLOT(sendData()));

    d->timeConnect->setInterval(ui->autoconnecttime->currentText().toInt());
    connect(ui->autoconnecttime,SIGNAL(currentIndexChanged(int)), this, SLOT(autCconnectRestart()));
    connect(d->timeConnect, SIGNAL(timeout()), this, SLOT(connectRestart()));
    connect(d->client, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(readError(QAbstractSocket::SocketError)));
    connect(d->client, SIGNAL(readyRead()), this, SLOT(clientRead()));

    connect(ui->savedata,&QPushButton::clicked,[this]
    {
        UiSet::saveToFile(ui->display,this);
    });
}

void TcpWidget::change(bool b)
{
    ui->model->setEnabled(b);
    ui->ip->setEnabled(b);
    ui->port->setEnabled(b);
    ui->sendbtn->setEnabled(!b);
    ui->autosend->setEnabled(!b);
}

void TcpWidget::openTcp()
{
    if(ui->listen->text()=="监听")
    {
        if(ui->port->text().isEmpty())
        {
            append(2,"端口号为空!");
            return;
        }
        d->ok = d->server->listen(QHostAddress::Any, quint16(ui->port->text().toInt()));
        change(!d->ok);
        ui->listen->setChecked(d->ok);
        if(d->ok)
        {
            ui->listen->setText("断开");
            //change(false);
            append(3,"监听成功!");
        }
        else
        {
            append(2,"监听失败，更换端口号,请重试!");
        }
    }
    else if(ui->listen->text()=="断开")
    {
        change(true);
        ui->autoconnect->setChecked(false);
        d->sendTime->stop();
        for(int i=0; i<d->serverClient.length(); i++)//断开所有连接
        {
            ui->allconnect->removeItem(ui->allconnect->findText(tr("%1:%2")\
                                                                .arg(d->serverClient[i]->peerAddress().toString().split("::ffff:")[1])\
                                       .arg(d->serverClient[i]->peerPort())));
            QString msg1=QString("%1[%2:%3]").arg("IP").arg(d->serverClient[i]->peerAddress().toString().split("::ffff:")[1])\
                    .arg(d->serverClient[i]->peerPort());
            append(4,msg1);
            //tcpClient123.at(i)->close();
            d->serverClient[i]->deleteLater();
            d->serverClient.removeAt(i);  //从保存的客户端列表中取去除
        }
        //->server->deleteLater();
        d->server->close();     //不再监听端口
        ui->listen->setText("监听");
        append(3,"监听关闭!");
        d->ok=false;
        ui->listen->setChecked(false);
    }
    else if(ui->listen->text()=="连接")
    {
        if(ui->port->text().isEmpty()||ui->port->text().isEmpty())
        {
            append(2,"服务器IP或者端口号为空!");
            return;
        }
        d->client->connectToHost(ui->ip->text(), quint16(ui->port->text().toInt()));
        if (d->client->waitForConnected(1000))  // 连接成功则进入if{}
        {
            ui->listen->setChecked(true);
            ui->listen->setText("断开连接");
            change(false);
            append(3,"连接到服务器成功!");
        }
        else
        {
            append(2,d->client->errorString());
            append(2,"连接到服务器失败,请确认服务器是否打开!");
        }
    }
    else if(ui->listen->text()=="断开连接")
    {
        d->client->disconnectFromHost();
        if (d->client->state() == QAbstractSocket::UnconnectedState|| d->client->waitForDisconnected(1000))  //已断开连接则进入if{}
        {
            ui->listen->setText("连接");
            ui->autoconnect->setChecked(false);
            change(true);
            d->timeConnect->stop();
            d->sendTime->stop();
            ui->autosend->setChecked(false);
            append(3,"断开连接成功!");
            ui->listen->setChecked(false);
        }
    }
}

// newConnection -> newConnectionSlot 新连接建立的槽函数
void TcpWidget::newConnection()
{
    d->serverCurrentClient = d->server->nextPendingConnection();
    d->serverClient.append(d->serverCurrentClient);
    ui->allconnect->addItem(tr("%1:%2").arg(d->serverCurrentClient->peerAddress().toString().split("::ffff:")[1])\
            .arg(d->serverCurrentClient->peerPort()));
    QString msg2=QString("%1[%2:%3]").arg("IP").arg(d->serverCurrentClient->peerAddress().toString().split("::ffff:")[1])\
            .arg(d->serverCurrentClient->peerPort());
    append(3,msg2);

    connect(d->serverCurrentClient, SIGNAL(readyRead()), this, SLOT(serverRead()));
    connect(d->serverCurrentClient, SIGNAL(disconnected()), this, SLOT(disconnected()));
}

// disconnected -> disconnectedSlot 客户端断开连接的槽函数
void TcpWidget::disconnected()
{
    QTcpSocket *currentClient=qobject_cast<QTcpSocket*>(sender());
    if(currentClient==nullptr) return;
    if(currentClient->state() == QAbstractSocket::UnconnectedState)
    {
        ui->allconnect->removeItem(ui->allconnect->findText(tr("%1:%2")\
                                                            .arg(currentClient->peerAddress().toString().split("::ffff:")[1])\
                                   .arg(currentClient->peerPort())));
        //
        QString msg3=QString("%1[%2:%3]").arg("IP").arg(currentClient->peerAddress().toString().split("::ffff:")[1])\
                .arg(currentClient->peerPort());
        append(4,msg3);
        // 删除存储在tcpClient123列表中的客户端信息
        currentClient->deleteLater();
        d->serverClient.removeOne(currentClient);
    }
}

void TcpWidget::readError(QAbstractSocket::SocketError)
{
    d->client->disconnectFromHost();
    ui->listen->setText(tr("连接"));
    change(true);
    d->sendTime->stop();
    ui->autosend->setChecked(false);
    append(2,QString("failed to connect server because %1,Please Try Again").arg(d->client->errorString()));
    append(2,"连接服务器失败，请重试!");
}

// 客户端数据可读信号，对应的读数据槽函数
void TcpWidget::serverRead()
{
    QTcpSocket *currentClient=qobject_cast<QTcpSocket*>(sender());
    if(currentClient==nullptr) return;
    QByteArray data=currentClient->readAll();
    if(data.isEmpty())    return;
    QString buffer;//储存数据
    if (ui->hexrecv->isChecked())
        buffer = BinaryCvn::byteArrayToHexStr(data);//16进制接受
    else
        //buffer = myHelper::byteArrayToAsciiStr(data);//ascii码接受
        buffer =data;
    static QString IP_Port;
    IP_Port = tr("[%1:%2]:").arg(currentClient->peerAddress().toString().split("::ffff:")[1])\
            .arg(currentClient->peerPort());

    QString buffer1=QString("%1%2%3").arg("IP ").arg(IP_Port).arg(buffer);

    append(1, buffer1);

    d->recvCount += data.size();//接收数据计数
    ui->recvcount->setText(QString("接收 : %1 字节").arg(d->recvCount));
}

void TcpWidget::sendData()
{
    QString data = ui->senddata->toPlainText();
    if (data.isEmpty()) {
        append(2,"发送数据为空！");
        ui->senddata->setFocus();
        return;
    }

    QByteArray buffer;
    if (ui->hexsend->isChecked()) {
        buffer = BinaryCvn::hexStrToByteArray(data);
    } else {
        //buffer = myHelper::asciiStrToByteArray(data);
        buffer=data.toUtf8();
    }
    append(0, data);
    d->sendCount += buffer.size();
    ui->sendcount->setText(QString("发送 : %1 字节").arg(d->sendCount));

    if(ui->model->currentIndex()==0)
    {
        if(d->serverClient.length()==0)
        {
            append(2,"当前无客户端在线，请停止发送数据,无效！");
            return;
        }
        //全部连接
        if(ui->allconnect->currentIndex() == 0)
        {
            for(int i=0; i<d->serverClient.length(); i++)
                d->serverClient[i]->write(buffer); //qt5除去了.toAscii()
        }
        //指定连接
        else
        {
            QString clientIP = ui->allconnect->currentText().split(":")[0];
            int clientPort = ui->allconnect->currentText().split(":")[1].toInt();
            //        qDebug() << clientIP;
            //        qDebug() << clientPort;
            for(int i=0; i<d->serverClient.length(); i++)
            {
                if(d->serverClient[i]->peerAddress().toString().split("::ffff:")[1]==clientIP\
                        && d->serverClient[i]->peerPort()==clientPort)
                {
                    d->serverClient[i]->write(buffer);
                    return; //ip:port唯一，无需继续检索
                }
            }
        }
    }
    else if(ui->model->currentIndex()==1)
    {
        d->client->write(buffer);
    }
}

void TcpWidget::clientRead()
{
    QByteArray data = d->client->readAll();
    if(data.isEmpty())    return;

    QString buffer;//储存数据
    if (ui->hexrecv->isChecked()) {
        buffer = BinaryCvn::byteArrayToHexStr(data);//16进制接受
    } else {
        //buffer = myHelper::byteArrayToAsciiStr(data);//ascii码接受
        buffer=data;
    }
    append(1, buffer);

    d->recvCount = data.size();//接收数据计数
    ui->recvcount->setText(QString("接收 : %1 字节").arg(d->recvCount));
}

void TcpWidget::on_autosend_stateChanged(int arg1)
{
    if (arg1 == 0)
        d->sendTime->stop();
    else
        d->sendTime->start();
}

void TcpWidget::autoSendRestart()
{
    if (ui->autosend->isChecked()){
        d->sendTime->stop();
        d->sendTime->setInterval(ui->autosendtime->currentText().toInt());
        d->sendTime->start();
    }
}

void TcpWidget::append(quint8 type, QString msg)
{   
    QString str;

    if (type == 0) {
        str = ">> 网络发送 :";
        ui->display->setTextColor(QColor("black"));
    }else if (type == 1) {
        str = "<< 网络接收 :";
        ui->display->setTextColor(QColor("dodgerblue"));
    }else if (type == 2) {
        str = "<< 提示信息 :";
        ui->display->setTextColor(QColor("red"));
    }else if (type == 3) {
        str = "<< 提示信息 :";
        ui->display->setTextColor(QColor("green"));
    }else if (type == 4) {
        str = ">> 断开链接:";
        ui->display->setTextColor(QColor("red"));
    }
    ui->display->append(QString("时间[%1] %2 %3").arg(DATETIME).arg(str).arg(msg));
}

void TcpWidget::autCconnectRestart()
{
    if (ui->autoconnect->isChecked()){
        d->timeConnect->stop();
        d->timeConnect->setInterval(ui->autoconnecttime->currentText().toInt());
        d->timeConnect->start();
    }
}

void TcpWidget::on_autoconnect_stateChanged(int arg1)
{
    if (arg1 == 0)
    {
        d->timeConnect->stop();
        if(ui->listen->text()=="连接")
        {
            change(true);
        }
        else if(ui->listen->text()=="断开连接")
        {
            change(false);
        }
    }
    else
    {
        d->timeConnect->start();
        ui->model->setEnabled(false);
        ui->ip->setEnabled(false);
        ui->port->setEnabled(false);
        if(ui->listen->text()=="连接")
        {
            ui->sendbtn->setEnabled(false);
            ui->autosend->setEnabled(false);
        }
        else if(ui->listen->text()=="断开连接")
        {
            ui->sendbtn->setEnabled(true);
            ui->autosend->setEnabled(true);
        }
    }
}

void TcpWidget::connectRestart()
{
    if(ui->listen->text()=="连接")
    {
        d->client->connectToHost(ui->ip->text(), quint16(ui->port->text().toInt()));
        if (d->client->waitForConnected(1000))  // 连接成功则进入if{}
        {
            ui->listen->setText("断开连接");
            change(false);
            append(3,"连接到服务器成功!");
        }
        else {
            append(2,"服务器未打开，连接到服务器失败！");
        }
    }
}

void TcpWidget::on_sendcount_clicked()
{
    d->sendCount = 0;
    ui->sendcount->setText("发送 : 0 字节");
}

void TcpWidget::on_recvcount_clicked()
{
    d->recvCount = 0;
    ui->recvcount->setText("接收 : 0 字节");
}

/*
void TcpWidget::on_savedata_clicked()
{
    QString tempData = ui->display->toPlainText();//以纯文本的形式返回文本编辑的文本。

    if (tempData == "")
    {
        append(2,"没有数据可以保存！");
        return;
    }

    QString path = QFileDialog::getSaveFileName(this,
                                                tr("Open File"),
                                                ".",
                                                tr("Text Files(*.txt)"));
    if(!path.isEmpty())
    {
        QFile file(path);
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QMessageBox::warning(this, tr("Write File"),
                                 tr("Can't open file:\n%1").arg(path));
            return;
        }
        QTextStream out(&file);
        out << tempData;
        file.close();
        append(3,"保存文件成功！");
    }
    else
    {
        append(2,"你没有保存任何文件！");
    }
}
*/
void TcpWidget::on_cleardata_clicked()
{
    ui->display->clear();
    d->sendCount = 0;
    ui->sendcount->setText("发送 : 0 字节");
    d->recvCount = 0;
    ui->recvcount->setText("接收 : 0 字节");
}

void TcpWidget::saveConfig()
{
    AppCfg::tcpModel=ui->model->currentText();
    AppCfg::tcpIp=ui->ip->text().toUtf8();
    AppCfg::tcpPort=ui->port->text().toInt();
    AppCfg::tcpHexRecv=ui->hexrecv->isChecked();
    AppCfg::tcpHexSend=ui->hexsend->isChecked();
    AppCfg::tcpAutoSendTime=ui->autosendtime->currentText().toInt();
    AppCfg::tcpAutoConnectTime=ui->autoconnecttime->currentText().toInt();
    AppCfg::tcpData=ui->senddata->toPlainText();
    AppCfg::writeConfig();
}
