#ifndef SERIALPORTTHREAD_H
#define SERIALPORTTHREAD_H

#include <QThread>

#include "serialparam.h"

class SerialPortThreadPrivate;
class SerialPortThread : public QThread
{
    Q_OBJECT
public:
    SerialPortThread(const SerialParam&, QObject *parent = nullptr);
    ~SerialPortThread() override;

signals:
    void serialOnLine(bool);
    void errorMessage(const QString&);
    void serialMessage(const QByteArray&);
    void sendMessage(const QByteArray&);

    void paramChanged(const SerialParam&);

protected:
    void run() override;

private:
    QScopedPointer<SerialPortThreadPrivate> d;
};

#endif // SERIALPORTTHREAD_H
