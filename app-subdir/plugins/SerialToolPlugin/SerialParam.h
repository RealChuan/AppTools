#ifndef SERIALPARAM_H
#define SERIALPARAM_H

#include <QSerialPort>

struct SerialParam {
    QString portName;
    int baudRate;
    QSerialPort::DataBits dataBits;
    QSerialPort::StopBits stopBits;
    QSerialPort::Parity parity;
    QSerialPort::FlowControl flowControl;
};

#endif // SERIALPARAM_H
