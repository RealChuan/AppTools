#include "useraccountsystem.h"
#include "accountquery.h"
#include "loginwidget.h"
#include "currentloginwidget.h"
#include "changepasswdwidget.h"
#include "registerwidget.h"

#include <utils/utils.h>

#include <QApplication>
#include <QSettings>
#include <QStackedWidget>

namespace AccountSystem {

struct Account{
    QString username;
    QString password;
};

class UserAccountSystemPrivate{
public:
    UserAccountSystemPrivate(QWidget *parent)
        : owner(parent){
        accountQuery = new AccountQuery(owner);

        changePasswdWidget = new ChangePasswdWidget(accountQuery, owner);
        currentLoginWidget = new CurrentLoginWidget(accountQuery, owner);
        loginWidget = new LoginWidget(accountQuery, owner);
        registerWidget = new RegisterWidget(accountQuery, owner);

        stackedWidget = new QStackedWidget(owner);
        stackedWidget->setObjectName("WhiteWidget");
        stackedWidget->addWidget(changePasswdWidget);
        stackedWidget->addWidget(currentLoginWidget);
        stackedWidget->addWidget(loginWidget);
        stackedWidget->addWidget(registerWidget);
    }
    QWidget* owner;
    Account currentAccount;
    bool autoLogin = false;
    AccountQuery *accountQuery;

    ChangePasswdWidget *changePasswdWidget;
    CurrentLoginWidget *currentLoginWidget;
    LoginWidget *loginWidget;
    RegisterWidget *registerWidget;
    QStackedWidget *stackedWidget;

    bool login = false;
};

UserAccountSystem::UserAccountSystem(QWidget *parent)
    : Dialog(parent)
    , d(new UserAccountSystemPrivate(this))
{
    setRestoreMaxButtonVisible(false);
    loadSetting();
    if(d->autoLogin && checkCurrentAccount())
        onLoginState(true);
    setupUI();
    buildConnect();
    resize(300, 480);
}

UserAccountSystem::~UserAccountSystem()
{
    saveSetting();
}

bool UserAccountSystem::loginState()
{
    return d->login;
}

void UserAccountSystem::onLogin()
{
    setTitle(tr("Login Widget"));
    onLoginState(false);
    d->loginWidget->clear();
    d->loginWidget->setNameList(d->accountQuery->userNameList());
    d->stackedWidget->setCurrentWidget(d->loginWidget);
}

void UserAccountSystem::onRegist()
{
    setTitle(tr("Register Widget"));
    d->stackedWidget->setCurrentWidget(d->registerWidget);
}

void UserAccountSystem::onChangedPassword()
{
    setTitle(tr("ChangePasswd Widget"));
    d->changePasswdWidget->clear();
    d->changePasswdWidget->setAccount(d->currentAccount.username,
                                      d->currentAccount.password);
    d->stackedWidget->setCurrentWidget(d->changePasswdWidget);
}

void UserAccountSystem::onCurrentLogin()
{
    setTitle(tr("Current Login Widget"));
    onLoginState(true);
    d->currentLoginWidget->setAccount(d->currentAccount.username,
                                      d->currentAccount.password);
    d->stackedWidget->setCurrentWidget(d->currentLoginWidget);
}

void UserAccountSystem::onComplete()
{
    d->currentAccount.username = d->loginWidget->username();
    d->currentAccount.password = d->loginWidget->password();
    d->autoLogin = d->loginWidget->autoLogin();
    onCurrentLogin();
}

void UserAccountSystem::onDeleteAccount()
{
    d->currentAccount.username.clear();
    d->currentAccount.password.clear();
    onLogin();
}

void UserAccountSystem::onLogout()
{
    d->currentAccount.username.clear();
    d->currentAccount.password.clear();
    onLogin();
}

void UserAccountSystem::onModifyPassword()
{
    d->currentAccount.password = d->changePasswdWidget->password();
    onCurrentLogin();
}

void UserAccountSystem::onRegister()
{
    d->currentAccount.username = d->registerWidget->username();
    d->currentAccount.password = d->registerWidget->password();
    onCurrentLogin();
}

void UserAccountSystem::onLoginState(bool state)
{
    d->login = state;
    emit login(state);
}

void UserAccountSystem::setupUI()
{
    setCentralWidget(d->stackedWidget);

    if(!checkCurrentAccount()){
        onLogin();
    }else{
        onCurrentLogin();
    }
}

void UserAccountSystem::buildConnect()
{
    connect(d->loginWidget, &LoginWidget::registered, this, &UserAccountSystem::onRegist);
    connect(d->loginWidget, &LoginWidget::complete, this, &UserAccountSystem::onComplete);

    connect(d->currentLoginWidget, &CurrentLoginWidget::changePassword, this, &UserAccountSystem::onChangedPassword);
    connect(d->currentLoginWidget, &CurrentLoginWidget::deleteAccount, this, &UserAccountSystem::onDeleteAccount);
    connect(d->currentLoginWidget, &CurrentLoginWidget::logout, this, &UserAccountSystem::onLogout);

    connect(d->changePasswdWidget, &ChangePasswdWidget::modifyPassword, this, &UserAccountSystem::onModifyPassword);
    connect(d->changePasswdWidget, &ChangePasswdWidget::cancel, this, &UserAccountSystem::onCurrentLogin);

    connect(d->registerWidget, &RegisterWidget::registered, this, &UserAccountSystem::onRegister);
    connect(d->registerWidget, &RegisterWidget::cancel, this, &UserAccountSystem::onCurrentLogin);
}

bool UserAccountSystem::checkCurrentAccount()
{
    if(d->currentAccount.username.isEmpty()
        || d->currentAccount.password.isEmpty())
        return false;
    if(d->accountQuery->checkAccount(d->currentAccount.username,
                                      d->currentAccount.password))
        return true;
    return false;
}

void UserAccountSystem::loadSetting()
{
    QSettings *setting = Utils::settings();
    if(!setting)
        return;

    setting->beginGroup("accout_config");
    d->autoLogin = setting->value("AutoLogin", false).toBool();
    if(d->autoLogin){
        d->currentAccount.username = setting->value("AutoLoginUsername").toString();
        d->currentAccount.password = setting->value("AutoLoginPassword").toString();
    }
    setting->endGroup();
}

void UserAccountSystem::saveSetting()
{
    QSettings *setting = Utils::settings();
    if(!setting)
        return;

    setting->beginGroup("accout_config");
    setting->setValue("AutoLogin", d->autoLogin);
    if(d->autoLogin){
        setting->setValue("AutoLoginUsername", d->currentAccount.username);
        setting->setValue("AutoLoginPassword", d->currentAccount.password);
    }
    setting->endGroup();
}

}
