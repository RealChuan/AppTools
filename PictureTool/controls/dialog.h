#ifndef DIALOG_H
#define DIALOG_H

#include "controls_global.h"
#include "commonwidget.h"

namespace Control {

struct DialogPrivate;
class CONTROLS_EXPORT Dialog : public CommonWidget
{
    Q_OBJECT
public:
    enum ExecFlags { Accepted, Rejected, Close };

    Dialog(QWidget *parent = nullptr);
    ~Dialog();

    ExecFlags exec();

public slots:
    void accept();
    void reject();

signals:
    void accepted();
    void rejected();

private slots:
    void onClosed();

private:
    void buildConnect();

    QScopedPointer<DialogPrivate> d_ptr;
};

}

#endif // DIALOG_H
