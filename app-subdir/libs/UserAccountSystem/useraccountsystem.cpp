#include "useraccountsystem.h"
#include "accountquery.h"
#include "loginwidget.h"
#include "currentloginwidget.h"

#include <extensionsystem/pluginmanager.h>

#include <QApplication>
#include <QSettings>

struct Account{
    QString username = QLatin1String("");
    QString password = QLatin1String("");
};

static AccountQuery *ACOUNTQuery = nullptr;

class UserAccountSystemPrivate{
public:
    UserAccountSystemPrivate(QObject *parent) : owner(parent){
        if(ACOUNTQuery){
            delete ACOUNTQuery;
            ACOUNTQuery = nullptr;
        }
        ACOUNTQuery = new AccountQuery(owner);
    }
    QObject* owner;
    Account currentAccount;
    QStringList usernameList;
    bool autoLogin = false;
};

UserAccountSystem::UserAccountSystem(QObject *parent) : QObject(parent)
  , d(new UserAccountSystemPrivate(this))
{
    loadSetting();
    if(d->autoLogin)
        if(checkCurrentAccount())
            emit login(true);
}

UserAccountSystem::~UserAccountSystem()
{
    if(ACOUNTQuery){
        delete ACOUNTQuery;
        ACOUNTQuery = nullptr;
    }
    saveSetting();
    delete d;
}

void UserAccountSystem::show()
{
    if(!checkCurrentAccount()){
        LoginWidget login(d->usernameList, qApp->activeWindow());
        if(login.exec() == LoginWidget::Accepted){
            d->currentAccount.username = login.username();
            d->currentAccount.password = login.password();
            d->autoLogin = login.autoLogin();
        }
        d->usernameList = login.usernameList();
    }
    if(checkCurrentAccount()){
        emit login(true);
        CurrentLoginWidget out(d->currentAccount.username,
                               d->currentAccount.password,
                               qApp->activeWindow());
        CurrentLoginWidget::ExecFlags flag = out.exec();
        if(flag == CurrentLoginWidget::Accepted){
            d->currentAccount.username.clear();
            d->currentAccount.password.clear();
            emit login(false);
        }else if(flag == CurrentLoginWidget::Rejected){
            d->usernameList.removeOne(d->currentAccount.username);
            d->currentAccount.username.clear();
            d->currentAccount.password.clear();
            emit login(false);
        }else if(flag == CurrentLoginWidget::Close){
            d->currentAccount.password = out.password();
            emit login(true);
        }
    }
}

AccountQuery *UserAccountSystem::accountQuery()
{
    return ACOUNTQuery;
}

bool UserAccountSystem::checkCurrentAccount()
{
    if(d->currentAccount.username.isEmpty()) return false;
    if(d->currentAccount.password.isEmpty()) return false;
    if(ACOUNTQuery->checkAccount(d->currentAccount.username,
                                 d->currentAccount.password))
        return true;
    return false;
}

void UserAccountSystem::loadSetting()
{
    QSettings *setting = ExtensionSystem::PluginManager::settings();
    if(!setting) return;

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
    QSettings *setting = ExtensionSystem::PluginManager::settings();
    if(!setting) return;

    setting->beginGroup("accout_config");
    setting->setValue("AcountList", d->usernameList);
    setting->setValue("AutoLogin", d->autoLogin);
    if(d->autoLogin){
        setting->setValue("AutoLoginUsername", d->currentAccount.username);
        setting->setValue("AutoLoginPassword", d->currentAccount.password);
    }
    setting->endGroup();
}
