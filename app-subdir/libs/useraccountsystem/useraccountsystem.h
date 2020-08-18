#ifndef USERACCOUNTSYSTEM_H
#define USERACCOUNTSYSTEM_H

#include "useraccountsystem_global.h"

#include <QObject>

namespace AccountSystem{

class AccountQuery;
class UserAccountSystemPrivate;
class USERACCOUNTSYSTEM_EXPORT UserAccountSystem : public QObject
{
    Q_OBJECT
public:
    UserAccountSystem(QObject *parent = nullptr);
    ~UserAccountSystem();

    void show();

signals:
    void login(bool);

private:
    bool checkCurrentAccount();

    void loadSetting();
    void saveSetting();

    QScopedPointer<UserAccountSystemPrivate> d;
};

}

#endif // USERACCOUNTSYSTEM_H
