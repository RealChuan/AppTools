#include "serialport.h"

class SerialPortPrivate{
public:
    SerialPortPrivate(QSerialPort *parent) : owner(parent){}

    QSerialPort *owner;
};

SerialPort::SerialPort(QObject *parent) : QSerialPort(parent)
{

}

SerialPort::~SerialPort()
{
    if(isOpen())
        close();
}

void SerialPort::openSerial()
{

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
