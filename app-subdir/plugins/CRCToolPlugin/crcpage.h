#ifndef CRCPAGE_H
#define CRCPAGE_H

#include <core/mpages.h>

class CRCPage : public MPages
{
    Q_OBJECT
public:
    CRCPage(QObject *parent = nullptr);

private:
    void init();
};

#endif // CRCPAGE_H
