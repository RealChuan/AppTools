#ifndef USERACCOUNTSYSTEM_H
#define USERACCOUNTSYSTEM_H

#include <controls/dialog.h>

using namespace Control;

namespace AccountSystem{

class AccountQuery;
class UserAccountSystemPrivate;
class UserAccountSystem : public Dialog
{
    Q_OBJECT
public:
    UserAccountSystem(QWidget *parent = nullptr);
    ~UserAccountSystem();

    bool loginState();

signals:
    void login(bool);

private slots:
    void onLogin();
    void onRegist();
    void onChangedPassword();
    void onCurrentLogin();
    void onComplete();
    void onDeleteAccount();
    void onLogout();
    void onModifyPassword();
    void onRegister();

private:
    void setupUI();
    void buildConnect();
    bool checkCurrentAccount();
    void onLoginState(bool state);

    void loadSetting();
    void saveSetting();

    QScopedPointer<UserAccountSystemPrivate> d;
};

}

#endif // USERACCOUNTSYSTEM_H
