#include "Sql.h"
#include "Json.h"

#include <QtWidgets>
#include <QDebug>
#include <QSqlError>

#pragma execution_character_set("utf-8")

dbSql::dbSql()
{
    Json json(QString("cfg/config.json"));
    QString type=json.getString("database","type");
    if("MYSQL"==type)
    {
        QString ip=json.getString("database","host");
        int port=json.getInt("database","port");
        QString databaseName=json.getString("database","databasename");
        QString userName=json.getString("database","username");
        QString passWord=json.getString("database","password");
        openMYSQL(ip,port,databaseName,userName,passWord);
    }
    else if("SQLITE"==type)
    {
        QString databaseName=json.getString("database","databasename");
        openSQLITE(databaseName);
    }
    else
        qDebug()<<"type error";
}

dbSql::~dbSql()
{
    if(dbSQL.open())
        dbSQL.close();
}

void dbSql::openMYSQL(QString ip, int port, QString databaseName, QString userName, QString passWord)
{
    dbSQL=QSqlDatabase::addDatabase("QMYSQL");
    //连接数据库主机名，这里需要注意（若填的为”127.0.0.1“，出现不能连接，则改为localhost)
    dbSQL.setHostName(ip);
    dbSQL.setPort(port);                    //连接数据库端口号
    dbSQL.setDatabaseName(databaseName);    //连接数据库名
    dbSQL.setUserName(userName);            //数据库用户名
    dbSQL.setPassword(passWord);            //数据库密码
    if(!dbSQL.open())
        qDebug()<<"不能连接"<<"connect to mysql error"<<dbSQL.lastError().text();
    else
        qDebug()<<"连接成功"<<"connect to mysql OK";
}

void dbSql::openSQLITE(QString databaseName)
{
    /*---------------------------------------------------------/
    /   在navicat中把数据加密了，但是用dbSQL.setPassWord()无效,   /
    /   可以打开数据库，但是查询失败,                             /
    /   QT的sql库中是否无法加密sqlite，                          /
    /   只能使用第三方插件或者自己实现加密功能                     /
    /---------------------------------------------------------*/
    dbSQL=QSqlDatabase::addDatabase("QSQLITE");
    QString dbname=QDir::currentPath() + "/"+databaseName;
    dbSQL.setDatabaseName(dbname);
    if(!dbSQL.open())
        qDebug()<<"不能连接"<<"connect to sqlite error"<<dbSQL.lastError().text();
    else
        qDebug()<<"连接成功"<<"connect to sqlite OK";
}

QSqlQuery dbSql::query(QString sql)
{
    QSqlQuery sqlquery;
    if(!sqlquery.exec(sql))
    {
        sqlquery.clear();
        qDebug()<<"query fail"<<sqlquery.lastError()<<endl;
    }
    return sqlquery;
}

QString dbSql::getString(QString sql, QString value)
{
    QSqlQuery sqlquery=query(sql);
    while(sqlquery.next())
        return sqlquery.value(value).toString();
    return "";
}
