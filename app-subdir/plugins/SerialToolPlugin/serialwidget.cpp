#include "serialwidget.h"

#include <QtWidgets>

class SerialWidgetPrivate{
public:
    SerialWidgetPrivate(QWidget *parent) : owner(parent){
        dataView = new QTextEdit(owner);
        sendData = new QTextEdit(owner);
        sendButton = new QPushButton(QObject::tr("Send"), owner);
        sendButton->setObjectName("SendButton");
        searchSerialButton = new QPushButton(QObject::tr("Detect Available Serial"), owner);
        serialBox = new QComboBox(owner);
        baudRateBox = new QComboBox(owner);
        dataBitsBox = new QComboBox(owner);
        stopBitsBox = new QComboBox(owner);
        parityBox = new QComboBox(owner);
        flowControlBox = new QComboBox(owner);
        openButton = new QPushButton(QObject::tr("Open Serial"), owner);
        openButton->setCheckable(true);
        hexBox = new QCheckBox(QObject::tr("HEX"), owner);
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
    QComboBox *serialBox;
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
}

SerialWidget::~SerialWidget()
{
    delete d;
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
    formLayout->addRow(tr("Serial: "), d->serialBox);
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
