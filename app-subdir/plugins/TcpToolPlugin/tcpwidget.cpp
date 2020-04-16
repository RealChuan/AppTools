#include "tcpwidget.h"
#include "tcpserverthread.h"
#include "tcpclientthread.h"

#include <utils/utils.h>
#include <controls/messbox.h>

#include <QHostAddress>
#include <QNetworkInterface>
#include <QtWidgets>

inline QString formatHex(const QByteArray &msg)
{
    QString temp;
    QString hex = QString::fromLocal8Bit(msg.toHex().toUpper());
    for (int i = 0; i < hex.length(); i = i + 2)
        temp += hex.mid(i, 2) + " ";    //两个字符+空格（例子：7e ）
    return temp;
}

enum Model { Server, Client };

class TcpWidgetPrivate{
public:
    TcpWidgetPrivate(QWidget *parent) : owner(parent){
        dataView = new QTextEdit(owner);
        dataView->document()->setMaximumBlockCount(1000);
        dataView->setReadOnly(true);

        sendData = new QTextEdit(owner);
        sendButton = new QPushButton(QObject::tr("Send"), owner);
        sendButton->setObjectName("SendButton");

        modelBox = new QComboBox(owner);
        modelBox->addItems(QStringList() << QObject::tr("TcpServer") << QObject::tr("TcpClient"));
        ipLabel = new QLabel(QObject::tr("Local IP List: "), owner);
        localIPBox = new QComboBox(owner);
        serverIPEdit = new QLineEdit(owner);
        QRegExp regExp("^((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)$");
        QValidator *validator = new QRegExpValidator(regExp, serverIPEdit);
        serverIPEdit->setValidator(validator);
        portLabel = new QLabel(QObject::tr("Local Port: "), owner);
        portEdit = new QLineEdit(owner);
        portEdit->setValidator(new QIntValidator(0, 65536, portEdit));
        listenOrConnectButton = new QPushButton(owner);
        listenOrConnectButton->setCheckable(true);

        hexBox = new QCheckBox(QObject::tr("Hex"), owner);
        autoSendBox = new QCheckBox(QObject::tr("Auto Delivery"), owner);
        autoSendTimeBox = new QSpinBox(owner);
        autoSendTimeBox->setSuffix(QObject::tr(" ms"));
        autoSendTimeBox->setRange(0, 10000);
        autoSendTimeBox->setValue(1000);
        autoSendTimeBox->setSingleStep(50);

        allConnectBox = new QComboBox(owner);
        allConnectBox->addItem(QObject::tr("Connect All"));
        autoConnectBox = new QCheckBox(QObject::tr("Auto Reconnect"), owner);
        autoConnectTimeBox = new QSpinBox(owner);
        autoConnectTimeBox->setSuffix(QObject::tr(" ms"));
        autoConnectTimeBox->setRange(1000, 100000);
        autoConnectTimeBox->setValue(1000);
        autoConnectTimeBox->setSingleStep(50);

        sendConutButton = new QPushButton(owner);
        recvConutButton = new QPushButton(owner);
        saveButton = new QPushButton(QObject::tr("Save Data"), owner);
        clearButton = new QPushButton(QObject::tr("Clear Screen"), owner);

        setWidget = new QWidget(owner);

        sendTime = new QTimer(owner);
        autoConnectTime = new QTimer(owner);
    }
    QWidget *owner;

    QTextEdit *dataView;
    QTextEdit *sendData;
    QPushButton *sendButton;

    QComboBox *modelBox;
    QLabel *ipLabel;
    QComboBox *localIPBox;
    QLineEdit *serverIPEdit;
    QLabel *portLabel;
    QLineEdit *portEdit;
    QPushButton *listenOrConnectButton;

    QCheckBox *hexBox;
    QCheckBox *autoSendBox;
    QSpinBox *autoSendTimeBox;
    QComboBox *allConnectBox;
    QCheckBox *autoConnectBox;
    QSpinBox *autoConnectTimeBox;

    QPushButton *sendConutButton;
    QPushButton *recvConutButton;
    QPushButton *saveButton;
    QPushButton *clearButton;

    QWidget *setWidget;

    TcpServerThread *serverThread = nullptr;
    TcpClientThread *clientThread = nullptr;

    QTimer *sendTime = nullptr;
    QTimer *autoConnectTime = nullptr;
    int sendCount = 0;
    int recvCount = 0;
};

TcpWidget::TcpWidget(QWidget *parent) : QWidget(parent)
  , d(new TcpWidgetPrivate(this))
{
    setupUI();
    init();
    buildConnect();
    onModelChange(tr("TcpServer"));
}

TcpWidget::~TcpWidget()
{
    delete d;
}

void TcpWidget::onModelChange(const QString &text)
{
    if(text == tr("TcpServer")){
        d->ipLabel->setText(tr("Local IP List: "));
        d->localIPBox->show();
        d->serverIPEdit->hide();
        d->portLabel->setText(tr("Local Port: "));
        d->listenOrConnectButton->setText(tr("Listen"));
        d->allConnectBox->show();
        d->autoConnectBox->hide();
        d->autoConnectTimeBox->hide();
        onServerOnline(false);
    }else if(text == tr("TcpClient")){
        d->ipLabel->setText(tr("Server IP: "));
        d->localIPBox->hide();
        d->serverIPEdit->show();
        d->listenOrConnectButton->setText(tr("Connect"));
        d->portLabel->setText(tr("Server Port: "));
        d->allConnectBox->hide();
        d->autoConnectBox->show();
        d->autoConnectTimeBox->show();
    }
    clearCount();
}

void TcpWidget::onListenOrConnect(bool state)
{
    d->listenOrConnectButton->setChecked(!state);
    QString port = d->portEdit->text();
    if(port.isEmpty()){
        MessBox::Warning(this, tr("Please enter the port number!"));
        d->portEdit->setFocus();
        return;
    }

    if(d->modelBox->currentText() == tr("TcpServer")){
        if(state) {
            destoryServerOrClientThread();
            d->serverThread = new TcpServerThread(quint16(port.toUInt()), d->localIPBox->currentText(), this);
            connect(d->serverThread, &TcpServerThread::serverOnLine, this, &TcpWidget::onServerOnline, Qt::UniqueConnection);
            connect(d->serverThread, &TcpServerThread::errorMessage, this, &TcpWidget::onAppendError, Qt::UniqueConnection);
            connect(d->serverThread, &TcpServerThread::newClientInfo, this, &TcpWidget::onServerNewClient, Qt::UniqueConnection);
            connect(d->serverThread, &TcpServerThread::disconnectClientInfo, this, &TcpWidget::onServerDisClient, Qt::UniqueConnection);
            connect(d->serverThread, &TcpServerThread::clientMessage, this, &TcpWidget::onServerRecvMessage, Qt::UniqueConnection);
            d->serverThread->start();
        } else {
            destoryServerOrClientThread();
            for(int i=1; i<d->allConnectBox->count(); i++){
                onServerDisClient(d->allConnectBox->itemText(i));
                d->allConnectBox->removeItem(i);
            }
        }
    }else if(d->modelBox->currentText() == tr("TcpClient")){
        if(state){
            destoryServerOrClientThread();
            createTcpClientThread();
        } else if(d->clientThread){
            destoryServerOrClientThread();
        }
    }
}

void TcpWidget::onSendData()
{
    QString str = d->sendData->toPlainText();
    if(str.isEmpty()) return;

    QByteArray bytes;
    if(d->hexBox->isChecked()){
        bytes = QByteArray::fromHex(str.toLocal8Bit()).toUpper();
        str = formatHex(bytes);
    } else
        bytes = str.toUtf8();

    if(d->serverThread){
        if(d->allConnectBox->count() == 1){
            QString error = tr("No client is currently online, please stop sending invalid!");
            appendDisplay(ErrorInfo, error);
            return;
        }
        QString clientInfo = d->allConnectBox->currentText();
        if(clientInfo == tr("Connect All")){
            appendDisplay(Send, QString(tr("Send To All Online Clients: %1.").arg(str)));
            clientInfo = "";
        } else
            appendDisplay(Send, QString(tr("Send To Clients [%1] : %2.").arg(clientInfo).arg(str)));
        emit d->serverThread->sendMessage(bytes, clientInfo);
        d->sendCount += str.size();
        setSendCount(d->sendCount);
    } else if(d->clientThread){
        emit d->clientThread->sendMessage(bytes);
        appendDisplay(Send, str);
        d->sendCount += bytes.size();
        setSendCount(d->sendCount);
    }
}

void TcpWidget::onServerOnline(bool state)
{
    d->modelBox->setEnabled(!state);
    d->localIPBox->setEnabled(!state);
    d->portEdit->setEnabled(!state);
    d->listenOrConnectButton->setChecked(state);
    d->listenOrConnectButton->setText(state? tr("Disconnect") : tr("Listen"));
    if(!state){
        d->autoSendBox->setChecked(state);
        d->sendTime->stop();
    }
    d->autoSendBox->setEnabled(state);
    d->sendButton->setEnabled(state);
    QString str;
    if(state){
        str = tr("Server Online!");
        appendDisplay(SuccessInfo, str);
    } else {
        str = tr("Server Offline!");
        appendDisplay(ErrorInfo, str);
    }
}

void TcpWidget::onServerNewClient(const QString &clientInfo)
{
    d->allConnectBox->addItem(clientInfo);
    QString str = clientInfo + tr(" Online.");
    appendDisplay(SuccessInfo, str);
}

void TcpWidget::onServerDisClient(const QString &clientInfo)
{
    d->allConnectBox->removeItem(d->allConnectBox->findText(clientInfo));
    QString str = clientInfo + tr(" Offline.");
    appendDisplay(ErrorInfo, str);
}

void TcpWidget::onServerRecvMessage(const QString &clientInfo, const QByteArray &bytes)
{
    if(bytes.isEmpty()) return;
    d->recvCount += bytes.size();
    setRecvCount(d->recvCount);
    QString str = clientInfo;
    if(d->hexBox->isChecked())
        str += formatHex(bytes);
    else
        str += QString::fromUtf8(bytes);
    appendDisplay(Recv, str);
}

void TcpWidget::onClientOnLine(bool state)
{
    d->modelBox->setEnabled(!state);
    d->serverIPEdit->setEnabled(!state);
    d->portEdit->setEnabled(!state);
    d->listenOrConnectButton->setChecked(state);
    d->listenOrConnectButton->setText(state? tr("Disconnect") : tr("Connect"));
    if(!state){
        d->autoSendBox->setChecked(state);
        d->sendTime->stop();
    }
    d->autoSendBox->setEnabled(state);
    d->sendButton->setEnabled(state);

    if(!state && !d->autoConnectBox->isChecked() && d->clientThread)
        destoryServerOrClientThread();

    QString str;
    if(state){
        str = tr("Client Online!");
        appendDisplay(SuccessInfo, str);
    } else {
        str = tr("Client Offline!");
        appendDisplay(ErrorInfo, str);
    }
}

void TcpWidget::onClientRecvMessage(const QByteArray &bytes)
{
    if(bytes.isEmpty()) return;
    d->recvCount += bytes.size();
    setRecvCount(d->recvCount);
    QString str;
    if(d->hexBox->isChecked())
        str = formatHex(bytes);
    else
        str = bytes;
    appendDisplay(Recv, str);
}

void TcpWidget::onAutoReconnectStartOrStop(bool state)
{
    d->setWidget->setEnabled(!state);
    if(state) {
        createTcpClientThread();
        d->autoConnectTime->start(d->autoConnectTimeBox->value());
    } else {
        d->autoConnectTime->stop();
        if(!d->listenOrConnectButton->isChecked())
            destoryServerOrClientThread();
    }
}

void TcpWidget::onAutoConnect()
{
    createTcpClientThread();
    emit d->clientThread->reconnect();
}

void TcpWidget::onAutoSend(bool state)
{
    if(state)
        d->sendTime->start(d->autoSendTimeBox->value());
    else
        d->sendTime->stop();
}

void TcpWidget::onSave()
{
    QString data = d->dataView->toPlainText();
    if(data.isEmpty()) return;

    QString path = QFileDialog::getSaveFileName(this, tr("Open File"),
                                                QString("./data/%1").arg(STRDATETIME),
                                                tr("Text Files(*.txt)"));
    if(!path.isEmpty()){
        QFile file(path);
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
            MessBox::Warning(this, tr("Write File: Can't open file:\n %1 !").arg(path));
            return;
        }
        QTextStream stream(&file);
        stream << data;
        file.close();
        appendDisplay(SuccessInfo, tr("The file was saved successfully."));
    }
    else
        appendDisplay(ErrorInfo, tr("No file saved."));
}

void TcpWidget::onAppendError(const QString &error)
{
    appendDisplay(ErrorInfo, error);
}

void TcpWidget::setupUI()
{
    QGroupBox *dataBox = new QGroupBox(tr("Data Display Window"), this);
    QHBoxLayout *dataLayout = new QHBoxLayout(dataBox);
    dataLayout->addWidget(d->dataView);

    QGroupBox *sendBox = new QGroupBox(tr("Data Sending Window"), this);
    sendBox->setObjectName("SendBox");
    QHBoxLayout *sendLayout = new QHBoxLayout(sendBox);
    sendLayout->addWidget(d->sendData);
    sendLayout->addWidget(d->sendButton);

    QSplitter *splitter = new QSplitter(Qt::Vertical, this);
    splitter->addWidget(dataBox);
    splitter->addWidget(sendBox);
    splitter->setHandleWidth(0);
    splitter->setSizes(QList<int>() << 1000 << 10);

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addWidget(splitter);

    QVBoxLayout *setLayout = new QVBoxLayout(d->setWidget);
    setLayout->setContentsMargins(0, 0, 0, 0);
    setLayout->addWidget(new QLabel(tr("Communication Mode: "), this));
    setLayout->addWidget(d->modelBox);
    setLayout->addWidget(d->ipLabel);
    setLayout->addWidget(d->localIPBox);
    setLayout->addWidget(d->serverIPEdit);
    setLayout->addWidget(d->portLabel);
    setLayout->addWidget(d->portEdit);
    setLayout->addWidget(d->listenOrConnectButton);

    QGroupBox *setBox = new QGroupBox(tr("Parameter Setting Window"), this);
    setBox->setObjectName("SetBox");
    QVBoxLayout *allSetLayout = new QVBoxLayout(setBox);
    allSetLayout->addWidget(d->setWidget);
    allSetLayout->addWidget(d->hexBox);
    allSetLayout->addWidget(d->autoSendBox);
    allSetLayout->addWidget(d->autoSendTimeBox);
    allSetLayout->addWidget(d->allConnectBox);
    allSetLayout->addWidget(d->autoConnectBox);
    allSetLayout->addWidget(d->autoConnectTimeBox);
    allSetLayout->addStretch(0);
    allSetLayout->addWidget(d->sendConutButton);
    allSetLayout->addWidget(d->recvConutButton);
    allSetLayout->addWidget(d->saveButton);
    allSetLayout->addWidget(d->clearButton);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addLayout(leftLayout);
    layout->addWidget(setBox);
}

void TcpWidget::init()
{
    d->localIPBox->clear();
    //  获得IP
    QList<QHostAddress> ipList = QNetworkInterface::allAddresses();
    for(QHostAddress address: ipList)
        if(address.protocol() == QAbstractSocket::IPv4Protocol)
            d->localIPBox->addItem(address.toString());
    d->localIPBox->setCurrentIndex(d->localIPBox->count() - 1);
    d->serverIPEdit->setText("127.0.0.1");
    d->portEdit->setText("65533");
}

void TcpWidget::buildConnect()
{
    connect(d->modelBox, &QComboBox::currentTextChanged, this, &TcpWidget::onModelChange);
    connect(d->listenOrConnectButton, &QPushButton::clicked, this, &TcpWidget::onListenOrConnect);

    QShortcut *sendShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Return), this);
    connect(sendShortcut, &QShortcut::activated, this, &TcpWidget::onSendData);
    connect(d->sendButton, &QPushButton::clicked, this, &TcpWidget::onSendData);

    connect(d->autoSendBox, &QCheckBox::clicked, this, &TcpWidget::onAutoSend);
    connect(d->sendTime, &QTimer::timeout, this, &TcpWidget::onSendData);

    connect(d->autoConnectBox, &QCheckBox::clicked, this, &TcpWidget::onAutoReconnectStartOrStop);
    connect(d->autoConnectTime, &QTimer::timeout, this, &TcpWidget::onAutoConnect);

    connect(d->sendConutButton, &QPushButton::clicked, [this]{ d->sendCount = 0; setSendCount(0); });
    connect(d->recvConutButton, &QPushButton::clicked, [this]{ d->recvCount = 0; setRecvCount(0); });
    connect(d->saveButton, &QPushButton::clicked, this, &TcpWidget::onSave);
    connect(d->clearButton, &QPushButton::clicked, d->dataView, &QTextEdit::clear);
}

void TcpWidget::clearCount()
{
    d->sendCount = 0;
    d->recvCount = 0;
    setSendCount(0);
    setRecvCount(0);
}

void TcpWidget::appendDisplay(TcpWidget::MessageType type, const QString &message)
{
    if(message.isEmpty()) return;

    QString display;
    if(type == Send) {
        display = tr(" >> Network Send: ");
        d->dataView->setTextColor(QColor("black"));
    } else if(type == Recv) {
        display = tr(" >> Network Recv: ");
        d->dataView->setTextColor(QColor("dodgerblue"));
    } else if(type == SuccessInfo) {
        display = tr(" >> Prompt Message: ");
        d->dataView->setTextColor(QColor("green"));
    } else if(type == ErrorInfo) {
        display = tr(" >> Prompt Message: ");
        d->dataView->setTextColor(QColor("red"));
    } else return;

    d->dataView->append(QString(tr("Time [%1] %2 %3").arg(STRDATETIMEMS).
                                arg(display).arg(message)));
}

void TcpWidget::setSendCount(int size)
{
    d->sendConutButton->setText(tr("Send: %1 Bytes").arg(size));
}

void TcpWidget::setRecvCount(int size)
{
    d->recvConutButton->setText(tr("Recv: %1 Bytes").arg(size));
}

void TcpWidget::createTcpClientThread()
{
    if(!d->clientThread){
        QString port = d->portEdit->text();
        if(port.isEmpty()){
            MessBox::Warning(this, tr("Please enter the port number!"));
            d->portEdit->setFocus();
            d->autoConnectBox->setChecked(false);
            onAutoReconnectStartOrStop(false);
            return;
        }
        QString ip = d->serverIPEdit->text().trimmed();
        if(ip.isEmpty()){
            MessBox::Warning(this, tr("Please enter the ip address!"));
            d->serverIPEdit->setFocus();
            d->autoConnectBox->setChecked(false);
            onAutoReconnectStartOrStop(false);
            return;
        }
        d->clientThread = new TcpClientThread(ip, quint16(port.toUInt()), this);
        connect(d->clientThread, &TcpClientThread::clientOnLine, this, &TcpWidget::onClientOnLine, Qt::UniqueConnection);
        connect(d->clientThread, &TcpClientThread::errorMessage, this, &TcpWidget::onAppendError, Qt::UniqueConnection);
        connect(d->clientThread, &TcpClientThread::serverMessage, this, &TcpWidget::onClientRecvMessage, Qt::UniqueConnection);
        d->clientThread->start();
    }
}

void TcpWidget::destoryServerOrClientThread()
{
    if(d->serverThread){
        delete d->serverThread;
        d->serverThread = nullptr;
    }
    if(d->clientThread){
        d->clientThread->deleteLater();
        d->clientThread = nullptr;
    }
}
