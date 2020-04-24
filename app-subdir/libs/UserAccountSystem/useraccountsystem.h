#ifndef USERACCOUNTSYSTEM_H
#define USERACCOUNTSYSTEM_H

#include "UserAccountSystem_global.h"

#include <QObject>

class AccountQuery;
class UserAccountSystemPrivate;
class USERACCOUNTSYSTEM_EXPORT UserAccountSystem : public QObject
{
public:
    UserAccountSystem(QObject *parent = nullptr);
    ~UserAccountSystem();

    void show();

    static AccountQuery *accountQuery();

private:
    UserAccountSystemPrivate *d;
};

#endif // USERACCOUNTSYSTEM_H
