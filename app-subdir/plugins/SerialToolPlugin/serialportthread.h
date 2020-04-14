#ifndef SERIALPORTTHREAD_H
#define SERIALPORTTHREAD_H

#include <QThread>

class SerialPortThread : public QThread
{
    Q_OBJECT
public:
    SerialPortThread(QObject *parent = nullptr);
    ~SerialPortThread() override;

protected:
    void run() override;
};

#endif // SERIALPORTTHREAD_H
