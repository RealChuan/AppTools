#ifndef DIALOG_H
#define DIALOG_H

#include "controls_global.h"
#include "commonwidget.h"

namespace Control {

class CONTROLS_EXPORT Dialog : public CommonWidget
{
    Q_OBJECT
public:
    enum ExecFlags { Accepted, Rejected, Close };

    Dialog(QWidget *parent = nullptr);

    ExecFlags exec();
    void accept();
    void reject();

signals:
    void accepted();
    void rejected();
};

}

#endif // DIALOG_H
