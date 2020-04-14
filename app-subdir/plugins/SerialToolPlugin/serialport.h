#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QSerialPort>

class SerialPortPrivate;
class SerialPort : public QSerialPort
{
    Q_OBJECT
public:
    SerialPort(QObject *parent = nullptr);
    ~SerialPort();

    void openSerial();

signals:
    void serialOnLine(bool);
    void errorMessage(const QString&);
    void serialMessage(const QByteArray&);

private slots:
    void onError();
    void onReadyRead();

private:
    void buildConnect();

    SerialPortPrivate *d;
};

#endif // SERIALPORT_H
