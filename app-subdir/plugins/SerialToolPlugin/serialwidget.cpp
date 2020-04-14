#include "serialwidget.h"

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QtWidgets>

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
        openButton = new QPushButton(QObject::tr("Open Serial"), owner);
        openButton->setCheckable(true);

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
    QPushButton *openButton;

    QCheckBox *hexBox;
    QCheckBox *autoSendBox;
    QSpinBox *autoSendTimeBox;
    QPushButton *sendConutButton;
    QPushButton *recvConutButton;
    QPushButton *saveButton;
    QPushButton *clearButton;
};

SerialWidget::SerialWidget(QWidget *parent) : QWidget(parent)
  , d(new SerialWidgetPrivate(this))
{
    setupUI();
    init();
    buildConnect();
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

void SerialWidget::onPortChanged(const QString &)
{

}

void SerialWidget::onBaudRateChanged(const QString &)
{

}

void SerialWidget::onDataBitsChanged(const QString &)
{

}

void SerialWidget::onStopBitsChanged(const QString &)
{

}

void SerialWidget::onParityChanged(const QString &)
{

}

void SerialWidget::onFlowControlChanged(const QString &)
{

}

void SerialWidget::onOpenOrCloseSerial(bool)
{

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
    setLayout->addWidget(d->openButton);
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

    connect(d->portBox, &QComboBox::currentTextChanged, this, &SerialWidget::onPortChanged);
    connect(d->baudRateBox, &QComboBox::currentTextChanged, this, &SerialWidget::onBaudRateChanged);
    connect(d->dataBitsBox, &QComboBox::currentTextChanged, this, &SerialWidget::onDataBitsChanged);
    connect(d->stopBitsBox, &QComboBox::currentTextChanged, this, &SerialWidget::onStopBitsChanged);
    connect(d->parityBox, &QComboBox::currentTextChanged, this, &SerialWidget::onParityChanged);
    connect(d->flowControlBox, &QComboBox::currentTextChanged, this, &SerialWidget::onFlowControlChanged);

    connect(d->openButton, &QPushButton::clicked, this, &SerialWidget::onOpenOrCloseSerial);
}
