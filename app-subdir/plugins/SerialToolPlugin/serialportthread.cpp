#include "serialportthread.h"

SerialPortThread::SerialPortThread(QObject *parent) : QThread(parent)
{

}

SerialPortThread::~SerialPortThread()
{
    if(isRunning()){
        quit();
        wait();
    }
}

void SerialPortThread::run()
{

}
