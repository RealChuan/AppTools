#ifndef LABELPAGE_H
#define LABELPAGE_H

#include <core/mpages.h>

using namespace Core;

class LabelPage : public MPages
{
    Q_OBJECT
public:
    LabelPage(QObject *parent = nullptr);

private:
    void init();
};

#endif // LABELPAGE_H
