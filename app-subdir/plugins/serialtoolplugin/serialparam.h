#ifndef SERIALPARAM_H
#define SERIALPARAM_H

#include <QSerialPort>

struct SerialParam {
    QString portName;
    QSerialPort::BaudRate baudRate    = QSerialPort::Baud9600;
    QSerialPort::DataBits dataBits    = QSerialPort::Data8;
    QSerialPort::Parity parity        = QSerialPort::NoParity;
    QSerialPort::StopBits stopBits    = QSerialPort::OneStop;
    QSerialPort::FlowControl flowControl = QSerialPort::NoFlowControl;
};

#endif // SERIALPARAM_H
