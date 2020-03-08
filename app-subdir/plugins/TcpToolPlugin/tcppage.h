#ifndef TCPPAGE_H
#define TCPPAGE_H

#include <core/mpages.h>

class TcpPage : public MPages
{
    Q_OBJECT
public:
    TcpPage(QObject *parent = nullptr);

private:
    void init();
};

#endif // TCPPAGE_H
