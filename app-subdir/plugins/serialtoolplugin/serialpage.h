#ifndef SERIALPAGE_H
#define SERIALPAGE_H

#include <core/mpages.h>

using namespace Core;

class SerialPage : public MPages
{
    Q_OBJECT
public:
    SerialPage(QObject *parent = nullptr);

private:
    void init();
};

#endif // SERIALPAGE_H
