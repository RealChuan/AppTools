#include "accountquery.h"

#include <utils/datbasesql.h>
#include <utils/json.h>

#include <QVariant>
#include <QDebug>
#include <QSqlError>

using namespace Utils;

namespace AccountSystem {

class AccountQueryPrivate{
public:
    AccountQueryPrivate(QObject *parent)
        : owner(parent){
        db = new DatabaseSQL(owner);
    }
    QObject *owner;
    DatabaseSQL *db;
    DatabaseParam dbParam;
    QStringList userNameList;
};

AccountQuery::AccountQuery(QObject *parent)
    : QObject(parent)
    , d(new AccountQueryPrivate(this))
{
    loadparam();
    if(!d->db->openSQL(d->dbParam)){
        qDebug() << d->db->errorString();
        return;
    }
    init();
    allUsername();
}

QStringList AccountQuery::userNameList() const
{
    return d->userNameList;
}

bool AccountQuery::contains(const QString &username)
{
    return d->userNameList.contains(username);
}

bool AccountQuery::checkAccount(const QString &username, const QString &password)
{
    QString sql = QLatin1String("SELECT PASSWORD FROM account "
                                "WHERE username = '%1'")
            .arg(username);
    QSqlQuery query = d->db->query(sql);
    if(query.next() && query.value("password").toString() == password)
        return true;
    qWarning() << query.lastError().text();
    return false;
}

bool AccountQuery::addAccount(const QString &username, const QString &password)
{
    QString sql = QLatin1String("INSERT INTO account (username, password) "
                                "VALUES('%1', '%2')")
            .arg(username)
            .arg(password);

    QSqlQuery query = d->db->query(sql);
    if(!query.lastError().isValid()){
        d->userNameList.append(username);
        return true;
    }
    qWarning() << query.lastError().text();
    return false;
}

bool AccountQuery::updateAccount(const QString &username, const QString &password)
{
    QString sql = QLatin1String("UPDATE account SET PASSWORD = '%1' "
                                "WHERE username = '%2'")
            .arg(password)
            .arg(username);

    QSqlQuery query = d->db->query(sql);
    if(!query.lastError().isValid())
        return true;
    qWarning() << query.lastError().text();
    return false;
}

bool AccountQuery::deleteAccount(const QString &username)
{
    QString sql = QLatin1String("DELETE FROM account WHERE username = '%1'")
            .arg(username);

    QSqlQuery query = d->db->query(sql);
    if(!query.lastError().isValid())
        return true;
    qWarning() << query.lastError().text();
    return false;
}

void AccountQuery::init()
{
    if(d->db->tableContains("account"))
        return;
    const QString createTable("CREATE TABLE [account]("
                              "[ID] INTEGER NOT NULL ON CONFLICT REPLACE UNIQUE ON CONFLICT REPLACE, "
                              "[username] VARCHAR(50) NOT NULL ON CONFLICT ROLLBACK UNIQUE ON CONFLICT ROLLBACK, "
                              "[password] VARCHAR(50) NOT NULL ON CONFLICT FAIL, "
                              "PRIMARY KEY([ID] COLLATE [BINARY] ASC));"
                              ""
                              "CREATE UNIQUE INDEX [name] ON [account]([username]);"
                              ""
                              "CREATE INDEX [username] ON [account]([username] COLLATE [BINARY] DESC);");

    QSqlQuery query = d->db->query(createTable);
    if(query.lastError().isValid())
        qWarning() << query.lastError().text();
}

void AccountQuery::loadparam()
{
    Json json("./config/config.json", true);
    d->dbParam.type = json.getValue("database.type").toString();
    d->dbParam.ip = json.getValue("database.host").toString();
    d->dbParam.port = json.getValue("database.port").toInt();
    d->dbParam.databaseName = json.getValue("database.databasename").toString();
    d->dbParam.uesrname = json.getValue("database.username").toString();
    d->dbParam.password = json.getValue("database.password").toString();
}

void AccountQuery::allUsername()
{
    QString sql = QLatin1String("SELECT username FROM account");
    QSqlQuery query = d->db->query(sql);
    d->userNameList.clear();
    while(query.next())
        d->userNameList.append(query.value("username").toString());
}

}
