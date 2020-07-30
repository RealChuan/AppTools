#ifndef DIALOG_H
#define DIALOG_H

#include "commonwidget.h"

class Dialog : public CommonWidget
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

#endif // DIALOG_H
