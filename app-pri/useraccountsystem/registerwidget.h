#ifndef REGISTERWIDGET_H
#define REGISTERWIDGET_H

#include <controls/dialog.h>

using namespace Control;

namespace AccountSystem {

class AccountQuery;
class RegisterWidgetPrivate;
class RegisterWidget : public Dialog
{
    Q_OBJECT
public:
    explicit RegisterWidget(AccountQuery*, QWidget *parent = nullptr);
    ~RegisterWidget();

    QString username() const;
    QString password() const;

signals:

private slots:
    void onRegister();

private:
    void setupUI();

    QScopedPointer<RegisterWidgetPrivate> d;
};

}

#endif // REGISTERWIDGET_H
