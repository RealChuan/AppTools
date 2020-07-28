#include "serialport.h"

#include <QDebug>

SerialPort::SerialPort(QObject *parent)
    : QSerialPort(parent)
{
    buildConnect();
}

SerialPort::~SerialPort()
{
    if(isOpen())
        close();
}

bool SerialPort::onOpenSerial(const SerialParam &param)
{
    if(isOpen()) close();

    bool ok = true;
    setPortName(param.portName);
    ok &= open(QIODevice::ReadWrite);
    flush();
    ok &= setBaudRate(static_cast<QSerialPort::BaudRate>(param.baudRate));
    ok &= setDataBits(static_cast<QSerialPort::DataBits>(param.dataBits));
    ok &= setStopBits(static_cast<QSerialPort::StopBits>(param.stopBits));
    ok &= setParity(static_cast<QSerialPort::Parity>(param.parity));
    ok &= setFlowControl(static_cast<QSerialPort::FlowControl>(param.flowControl));

    if(!ok) onError();

    emit serialOnLine(ok);

    return ok;
}

void SerialPort::onWrite(const QByteArray &bytes)
{
    if(!isOpen()) return;

    write(bytes);
    waitForBytesWritten(100);
}

void SerialPort::onError()
{
    if(error() == QSerialPort::NoError) return;

    QString error = tr("Serial Error: %1").arg(errorString());
    emit errorMessage(error);
}

void SerialPort::onReadyRead()
{
    if(bytesAvailable() <= 0 ) return;

    QByteArray bytes;
    while (!atEnd())
        bytes += readAll();

    emit serialMessage(bytes);
}

void SerialPort::buildConnect()
{
    connect(this, &SerialPort::errorOccurred, this ,&SerialPort::onError);
    connect(this, &SerialPort::readyRead, this, &SerialPort::onReadyRead);
}
