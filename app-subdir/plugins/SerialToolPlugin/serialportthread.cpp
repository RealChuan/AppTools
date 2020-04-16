#include "serialportthread.h"
#include "serialport.h"

class SerialPortThreadPrivate{
public:
    SerialPortThreadPrivate(QThread *parent) : owner(parent){}

    QThread *owner;
    SerialParam serialParam;
};

SerialPortThread::SerialPortThread(const SerialParam &serialParam, QObject *parent) : QThread(parent)
  , d(new SerialPortThreadPrivate(this))
{
    d->serialParam = serialParam;
}

SerialPortThread::~SerialPortThread()
{
    if(isRunning()){
        quit();
        wait();
    }
    delete d;
}

void SerialPortThread::run()
{
    QScopedPointer<SerialPort> serialPort(new SerialPort);
    connect(serialPort.data(), &SerialPort::serialOnLine, this, &SerialPortThread::serialOnLine);
    connect(serialPort.data(), &SerialPort::errorMessage, this, &SerialPortThread::errorMessage);
    connect(serialPort.data(), &SerialPort::serialMessage, this, &SerialPortThread::serialMessage);
    connect(this, &SerialPortThread::sendMessage, serialPort.data(), &SerialPort::onWrite);
    connect(this, &SerialPortThread::paramChanged, serialPort.data(), &SerialPort::onOpenSerial);

    bool ok = serialPort->onOpenSerial(d->serialParam);
    if(!ok) return;

    exec();

    emit serialOnLine(false);
}
