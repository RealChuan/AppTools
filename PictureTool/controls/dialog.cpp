#include "dialog.h"

#include <QEventLoop>

namespace Control {

Dialog::Dialog(QWidget *parent)
    : CommonWidget(parent)
{
    setMinButtonVisible(false);
    setRestoreMaxButtonVisible(false);
    resize(600, 370);
}

Dialog::ExecFlags Dialog::exec()
{
    setWindowFlags(Qt::Dialog | Qt::Popup | Qt::FramelessWindowHint);
    setWindowModality(Qt::ApplicationModal);
    show();
    ExecFlags flag = Rejected;
    QEventLoop loop(this);
    connect(this, &Dialog::accepted, [&loop, &flag]{
        flag = Accepted;
        loop.quit();
    });
    connect(this, &Dialog::rejected, [&loop, &flag]{
        flag = Rejected;
        loop.quit();
    });
    connect(this, &Dialog::aboutToclose, [&loop, &flag]{
        flag = Close;
        loop.quit();
    });
    loop.exec();

    return flag;
}

void Dialog::accept()
{
    emit accepted();
}

void Dialog::reject()
{
    emit rejected();
}

}
