#include "useraccountsystem.h"
#include "accountquery.h"
#include "loginwidget.h"
#include "currentloginwidget.h"

#include <utils/utils.h>

#include <QApplication>
#include <QSettings>

struct Account{
    QString username;
    QString password;
};

class UserAccountSystemPrivate{
public:
    UserAccountSystemPrivate(QObject *parent)
        : owner(parent){
        accountQuery = new AccountQuery(owner);
    }
    QObject* owner;
    Account currentAccount;
    QStringList usernameList;
    bool autoLogin = false;
    AccountQuery *accountQuery;
};

UserAccountSystem::UserAccountSystem(QObject *parent)
    : QObject(parent)
    , d(new UserAccountSystemPrivate(this))
{
    loadSetting();
    if(d->autoLogin && checkCurrentAccount())
        emit login(true);
}

UserAccountSystem::~UserAccountSystem()
{
    saveSetting();
}

void UserAccountSystem::show()
{
    if(!checkCurrentAccount()){
        LoginWidget login(d->accountQuery, d->usernameList, qApp->activeWindow());
        if(login.exec() == LoginWidget::Accepted){
            d->currentAccount.username = login.username();
            d->currentAccount.password = login.password();
            d->autoLogin = login.autoLogin();
        }
        d->usernameList = login.usernameList();
    }
    if(checkCurrentAccount()){
        emit login(true);
        CurrentLoginWidget out(d->accountQuery, d->currentAccount.username,
                               d->currentAccount.password, qApp->activeWindow());
        switch (out.exec()) {
        case CurrentLoginWidget::Accepted:
            d->currentAccount.username.clear();
            d->currentAccount.password.clear();
            emit login(false);
            break;
        case CurrentLoginWidget::Rejected:
            d->usernameList.removeOne(d->currentAccount.username);
            d->currentAccount.username.clear();
            d->currentAccount.password.clear();
            emit login(false);
            break;
        case CurrentLoginWidget::Close:
            d->currentAccount.password = out.password();
            emit login(true);
            break;
        default: break;
        }
    }
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
    d->usernameList = setting->value("AcountList").toStringList();
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
    setting->setValue("AcountList", d->usernameList);
    setting->setValue("AutoLogin", d->autoLogin);
    if(d->autoLogin){
        setting->setValue("AutoLoginUsername", d->currentAccount.username);
        setting->setValue("AutoLoginPassword", d->currentAccount.password);
    }
    setting->endGroup();
}
