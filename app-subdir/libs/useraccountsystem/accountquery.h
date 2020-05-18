#ifndef SQLQUERY_H
#define SQLQUERY_H

#include <QObject>

class AccountQueryPrivate;
class AccountQuery : public QObject
{
public:
    AccountQuery(QObject *parent = nullptr);

    bool contains(const QString&);
    bool checkAccount(const QString&, const QString&);
    bool addAccount(const QString&, const QString&);
    bool updateAccount(const QString&, const QString&);
    bool deleteAccount(const QString&);

private:
    void loadparam();
    void allUsername();

    QScopedPointer<AccountQueryPrivate> d;
};

#endif // SQLQUERY_H
