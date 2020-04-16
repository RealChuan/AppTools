#include "serialwidget.h"
#include "serialportthread.h"
#include "serialparam.h"

#include <utils/utils.h>
#include <controls/messbox.h>

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QtWidgets>

inline QString formatHex(const QByteArray &msg)
{
    QString temp;
    QString hex = QString::fromLocal8Bit(msg.toHex().toUpper());
    for (int i = 0; i < hex.length(); i = i + 2)
        temp += hex.mid(i, 2) + " ";    //两个字符+空格（例子：7e ）
    return temp;
}

class SerialWidgetPrivate{
public:
    SerialWidgetPrivate(QWidget *parent) : owner(parent){
        dataView = new QTextEdit(owner);
        dataView->document()->setMaximumBlockCount(1000);
        dataView->setReadOnly(true);

        sendData = new QTextEdit(owner);
        sendButton = new QPushButton(QObject::tr("Send"), owner);
        sendButton->setObjectName("SendButton");

        searchSerialButton = new QPushButton(QObject::tr("Search Available Serial"), owner);
        portBox = new QComboBox(owner);
        baudRateBox = new QComboBox(owner);
        dataBitsBox = new QComboBox(owner);
        stopBitsBox = new QComboBox(owner);
        parityBox = new QComboBox(owner);
        flowControlBox = new QComboBox(owner);
        openOrCloseButton = new QPushButton(QObject::tr("Open Serial"), owner);
        openOrCloseButton->setCheckable(true);

        hexBox = new QCheckBox(QObject::tr("Hex"), owner);
        autoSendBox = new QCheckBox(QObject::tr("Auto Delivery"), owner);
        autoSendTimeBox = new QSpinBox(owner);
        autoSendTimeBox->setSuffix(QObject::tr(" ms"));
        autoSendTimeBox->setRange(0, 10000);
        autoSendTimeBox->setValue(1000);
        autoSendTimeBox->setSingleStep(50);

        sendConutButton = new QPushButton(QObject::tr("Send: 0 Bytes"), owner);
        recvConutButton = new QPushButton(QObject::tr("Receive: 0 Bytes"), owner);
        saveButton = new QPushButton(QObject::tr("Save Data"), owner);
        clearButton = new QPushButton(QObject::tr("Clear Screen"), owner);

        sendTime = new QTimer(owner);
    }
    QWidget *owner;

    QTextEdit *dataView;
    QTextEdit *sendData;
    QPushButton *sendButton;

    QPushButton *searchSerialButton;
    QComboBox *portBox;
    QComboBox *baudRateBox;
    QComboBox *dataBitsBox;
    QComboBox *stopBitsBox;
    QComboBox *parityBox;
    QComboBox *flowControlBox;
    QPushButton *openOrCloseButton;

    QCheckBox *hexBox;
    QCheckBox *autoSendBox;
    QSpinBox *autoSendTimeBox;
    QPushButton *sendConutButton;
    QPushButton *recvConutButton;
    QPushButton *saveButton;
    QPushButton *clearButton;

    SerialPortThread *serialThread = nullptr;
    SerialParam serialParam;

    QTimer *sendTime = nullptr;
    int sendCount = 0;
    int recvCount = 0;
};

SerialWidget::SerialWidget(QWidget *parent) : QWidget(parent)
  , d(new SerialWidgetPrivate(this))
{
    qRegisterMetaType<SerialParam>("SerialParam");
    setupUI();
    init();
    buildConnect();
    onSerialOnline(false);
}

SerialWidget::~SerialWidget()
{
    delete d;
}

void SerialWidget::onSearchPort()
{
    d->portBox->clear();

    // 查找可用的串口
    for(const QSerialPortInfo &info: QSerialPortInfo::availablePorts()){
        QSerialPort port;
        port.setPort(info);
        if(port.open(QIODevice::ReadWrite)) {
            d->portBox->addItem(port.portName());     //将串口号添加到portname
            port.close();       //关闭串口等待人为(打开串口按钮)打开
        }
    }
}

void SerialWidget::onSendData()
{
    QString str = d->sendData->toPlainText();
    if(str.isEmpty()) return;

    QByteArray bytes;
    if(d->hexBox->isChecked()){
        bytes = QByteArray::fromHex(str.toLocal8Bit()).toUpper();
        str = formatHex(bytes);
    } else
        bytes = str.toLatin1();

    if(d->serialThread){
        emit d->serialThread->sendMessage(bytes);
        appendDisplay(Send, str);
        d->sendCount += bytes.size();
        setSendCount(d->sendCount);
    }
}

void SerialWidget::onParamChanged(const QString &)
{
    if(!d->serialThread) return;

    setSerialParam();
    emit d->serialThread->paramChanged(d->serialParam);
}

void SerialWidget::onOpenOrCloseSerial(bool state)
{
    d->openOrCloseButton->setChecked(!state);
    if(state){
        destorySerialThread();
        setSerialParam();
        d->serialThread = new SerialPortThread(d->serialParam, this);
        connect(d->serialThread, &SerialPortThread::serialOnLine, this, &SerialWidget::onSerialOnline, Qt::UniqueConnection);
        connect(d->serialThread, &SerialPortThread::errorMessage, this, &SerialWidget::onAppendError, Qt::UniqueConnection);
        connect(d->serialThread, &SerialPortThread::serialMessage, this, &SerialWidget::onSerialRecvMessage, Qt::UniqueConnection);
        d->serialThread->start();
    } else {
        destorySerialThread();
    }
}

void SerialWidget::onSerialOnline(bool state)
{
    d->searchSerialButton->setEnabled(!state);
    d->openOrCloseButton->setChecked(state);
    d->openOrCloseButton->setText(state? tr("Close Serial") : tr("Open Serial"));

    if(!state){
        d->autoSendBox->setChecked(state);
        d->sendTime->stop();
    }
    d->autoSendBox->setEnabled(state);
    d->sendButton->setEnabled(state);
    QString str;
    if(state){
        str = tr("Serial Open!");
        appendDisplay(SuccessInfo, str);
    } else {
        str = tr("Serial Close!");
        appendDisplay(ErrorInfo, str);
    }
}

void SerialWidget::onAppendError(const QString &error)
{
    appendDisplay(ErrorInfo, error);
}

void SerialWidget::onSerialRecvMessage(const QByteArray &bytes)
{
    //qDebug() << "onSerialRecvMessage: " << bytes;
    if(bytes.isEmpty()) return;
    d->recvCount += bytes.size();
    setRecvCount(d->recvCount);
    QString str;
    if(d->hexBox->isChecked())
        str = formatHex(bytes);
    else
        str = QString::fromLatin1(bytes);
    appendDisplay(Recv, str);
}

void SerialWidget::onAutoSend(bool state)
{
    if(state)
        d->sendTime->start(d->autoSendTimeBox->value());
    else
        d->sendTime->stop();
}

void SerialWidget::onSave()
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

void SerialWidget::setupUI()
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

    QFormLayout *formLayout = new QFormLayout;
    formLayout->setContentsMargins(0, 0, 0, 0);
    formLayout->addRow(tr("Port: "), d->portBox);
    formLayout->addRow(tr("Baud Rate: "), d->baudRateBox);
    formLayout->addRow(tr("Data Bits: "), d->dataBitsBox);
    formLayout->addRow(tr("Stop Bits: "), d->stopBitsBox);
    formLayout->addRow(tr("Parity: "), d->parityBox);
    formLayout->addRow(tr("Flow Control: "), d->flowControlBox);

    QGroupBox *setBox = new QGroupBox(tr("Parameter Setting Window"), this);
    setBox->setObjectName("SetBox");
    QVBoxLayout *setLayout = new QVBoxLayout(setBox);
    setLayout->addWidget(d->searchSerialButton);
    setLayout->addLayout(formLayout);
    setLayout->addWidget(d->openOrCloseButton);
    setLayout->addWidget(d->hexBox);
    setLayout->addWidget(d->autoSendBox);
    setLayout->addWidget(d->autoSendTimeBox);
    setLayout->addStretch(0);
    setLayout->addWidget(d->sendConutButton);
    setLayout->addWidget(d->recvConutButton);
    setLayout->addWidget(d->saveButton);
    setLayout->addWidget(d->clearButton);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addLayout(leftLayout);
    layout->addWidget(setBox);
}

void SerialWidget::init()
{
    onSearchPort();
    QList<qint32> baudList = QSerialPortInfo::standardBaudRates();
    for(const qint32 baudrate: baudList){
        d->baudRateBox->addItem(QString::number(baudrate));
    }
    d->baudRateBox->setCurrentIndex(6);

    d->dataBitsBox->addItem("5", QSerialPort::Data5);
    d->dataBitsBox->addItem("6", QSerialPort::Data6);
    d->dataBitsBox->addItem("7", QSerialPort::Data7);
    d->dataBitsBox->addItem("8", QSerialPort::Data8);
    d->dataBitsBox->setCurrentIndex(3);

    d->stopBitsBox->addItem("1",   QSerialPort::OneStop);
#ifdef Q_OS_WIN
    d->stopBitsBox->addItem("1.5", QSerialPort::OneAndHalfStop);
#endif
    d->stopBitsBox->addItem("2",   QSerialPort::TwoStop);

    d->parityBox->addItem(tr("No"), QSerialPort::NoParity);
    d->parityBox->addItem(tr("Even"), QSerialPort::EvenParity);
    d->parityBox->addItem(tr("Odd"),  QSerialPort::OddParity);

    d->flowControlBox->addItem(tr("No"),     QSerialPort::NoFlowControl);
    d->flowControlBox->addItem(tr("Hardware"), QSerialPort::HardwareControl);
    d->flowControlBox->addItem(tr("Software"), QSerialPort::SoftwareControl);
}

void SerialWidget::buildConnect()
{
    connect(d->searchSerialButton, &QPushButton::clicked, this, &SerialWidget::onSearchPort);

    connect(d->portBox, &QComboBox::currentTextChanged, this, &SerialWidget::onParamChanged);
    connect(d->baudRateBox, &QComboBox::currentTextChanged, this, &SerialWidget::onParamChanged);
    connect(d->dataBitsBox, &QComboBox::currentTextChanged, this, &SerialWidget::onParamChanged);
    connect(d->stopBitsBox, &QComboBox::currentTextChanged, this, &SerialWidget::onParamChanged);
    connect(d->parityBox, &QComboBox::currentTextChanged, this, &SerialWidget::onParamChanged);
    connect(d->flowControlBox, &QComboBox::currentTextChanged, this, &SerialWidget::onParamChanged);

    connect(d->openOrCloseButton, &QPushButton::clicked, this, &SerialWidget::onOpenOrCloseSerial);

    QShortcut *sendShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Return), this);
    connect(sendShortcut, &QShortcut::activated, this, &SerialWidget::onSendData);
    connect(d->sendButton, &QPushButton::clicked, this, &SerialWidget::onSendData);

    connect(d->autoSendBox, &QCheckBox::clicked, this, &SerialWidget::onAutoSend);
    connect(d->sendTime, &QTimer::timeout, this, &SerialWidget::onSendData);

    connect(d->sendConutButton, &QPushButton::clicked, [this]{ d->sendCount = 0; setSendCount(0); });
    connect(d->recvConutButton, &QPushButton::clicked, [this]{ d->recvCount = 0; setRecvCount(0); });
    connect(d->saveButton, &QPushButton::clicked, this, &SerialWidget::onSave);
    connect(d->clearButton, &QPushButton::clicked, d->dataView, &QTextEdit::clear);
}

void SerialWidget::setSerialParam()
{
    d->serialParam.portName = d->portBox->currentText();
    d->serialParam.baudRate = QSerialPort::BaudRate(d->baudRateBox->currentText().toInt());
    d->serialParam.dataBits = QSerialPort::DataBits(d->dataBitsBox->currentData().toInt());
    d->serialParam.stopBits = QSerialPort::StopBits(d->stopBitsBox->currentData().toInt());
    d->serialParam.parity = QSerialPort::Parity(d->parityBox->currentData().toInt());
    d->serialParam.flowControl = QSerialPort::FlowControl(d->flowControlBox->currentData().toInt());
}

void SerialWidget::destorySerialThread()
{
    if(d->serialThread){
        delete d->serialThread;
        d->serialThread = nullptr;
    }
}

void SerialWidget::appendDisplay(SerialWidget::MessageType type, const QString & message)
{
    if(message.isEmpty()) return;

    QString display;
    if(type == Send) {
        display = tr(" >> Serial Send: ");
        d->dataView->setTextColor(QColor("black"));
    } else if(type == Recv) {
        display = tr(" >> Serial Recv: ");
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

void SerialWidget::setSendCount(int size)
{
    d->sendConutButton->setText(tr("Send: %1 Bytes").arg(size));
}

void SerialWidget::setRecvCount(int size)
{
    d->recvConutButton->setText(tr("Recv: %1 Bytes").arg(size));
}
