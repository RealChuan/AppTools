#include "useraccountsystem.h"
#include "accountquery.h"
#include "loginwidget.h"
#include "logoutordeletewidget.h"

#include <QApplication>

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
};

UserAccountSystem::UserAccountSystem(QObject *parent) : QObject(parent)
  , d(new UserAccountSystemPrivate(this))
{

}

UserAccountSystem::~UserAccountSystem()
{
    delete d;
}

void UserAccountSystem::show()
{
    if(d->currentAccount.username.isEmpty()){
        LoginWidget login(d->usernameList, qApp->activeWindow());
        if(login.exec() == LoginWidget::Accepted){
            d->currentAccount.username = login.username();
            d->currentAccount.password = login.password();
            d->usernameList = login.usernameList();
        }
    }else{
        LogoutOrDeleteWidget out(d->currentAccount.username,
                                 d->currentAccount.password,
                                 qApp->activeWindow());
        LogoutOrDeleteWidget::ExecFlags flag = out.exec();
        if(flag == LogoutOrDeleteWidget::Accepted){
            d->currentAccount.username.clear();
            d->currentAccount.password.clear();
        }else if(flag == LogoutOrDeleteWidget::Rejected){
            d->usernameList.removeOne(d->currentAccount.username);
            d->currentAccount.username.clear();
            d->currentAccount.password.clear();
        }
    }
}

AccountQuery *UserAccountSystem::accountQuery()
{
    return ACOUNTQuery;
}
