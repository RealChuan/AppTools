#include "dialog.h"

#include <QEventLoop>

namespace Control {

struct DialogPrivate{
    QEventLoop loop;
    Dialog::ExecFlags flag = Dialog::ExecFlags::Close;
};

Dialog::Dialog(QWidget *parent)
    : CommonWidget(parent)
    , d_ptr(new DialogPrivate)
{
    setMinButtonVisible(false);
    setRestoreMaxButtonVisible(false);
    buildConnect();
    resize(600, 370);
}

Dialog::~Dialog()
{

}

Dialog::ExecFlags Dialog::exec()
{
    setWindowFlags(Qt::Dialog | Qt::Popup | Qt::FramelessWindowHint);
    setWindowModality(Qt::ApplicationModal);
    show();
    raise();
    activateWindow();

    d_ptr->flag = Close;
    d_ptr->loop.exec();

    hide();

    return d_ptr->flag;
}

void Dialog::accept()
{
    d_ptr->flag = Accepted;
    d_ptr->loop.quit();
}

void Dialog::reject()
{
    d_ptr->flag = Rejected;
    d_ptr->loop.quit();
}

void Dialog::onClosed()
{
    d_ptr->flag = Close;
    d_ptr->loop.quit();
}

void Dialog::buildConnect()
{
    connect(this, &Dialog::accepted, this, &Dialog::accept);
    connect(this, &Dialog::rejected, this, &Dialog::reject);
    connect(this, &Dialog::aboutToclose, this, &Dialog::onClosed);
}

}
