#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include "UserAccountSystem_global.h"

#include <controls/dialog.h>

class LoginWidgetPrivate;
class USERACCOUNTSYSTEM_EXPORT LoginWidget : public Dialog
{
    Q_OBJECT
public:
    LoginWidget(QWidget *parent = nullptr);
    ~LoginWidget();

private slots:
    void onLogin();
    void onRegister();
    void onChangePasswd();

private:
    void setupUI();
    void buildConnect();

    LoginWidgetPrivate *d;
};

#endif // LOGINWIDGET_H
