#include "MySql.h"

#include <QDebug>
#include <QSqlError>

#pragma execution_character_set("utf-8")

MySql::MySql(QString ip,int port,QString databaseName,QString userName,QString passWord)
{
    dbMySql=QSqlDatabase::addDatabase("QMYSQL");
    dbMySql.setHostName(ip);      //连接数据库主机名，这里需要注意（若填的为”127.0.0.1“，出现不能连接，则改为localhost)
    dbMySql.setPort(port);                 //连接数据库端口号，与设置一致
    dbMySql.setDatabaseName(databaseName);     //连接数据库名，与设置一致
    dbMySql.setUserName(userName);           //数据库用户名，与设置一致
    dbMySql.setPassword(passWord);           //数据库密码，与设置一致
    if(!dbMySql.open())
        qDebug()<<"不能连接"<<"connect to mysql error"<<dbMySql.lastError().text();
    else
        qDebug()<<"连接成功"<<"connect to mysql OK";
}

MySql::~MySql()
{
    if(dbMySql.open())
        dbMySql.close();
}

void MySql::query(QString sql)
{
    QSqlQuery sqlquery;
    if(!sqlquery.exec(sql))
        qDebug()<<"query fail"<<sqlquery.lastError()<<endl;
    sqlquery.clear();
}

QVector<QStringList> MySql::query(QString sql, QStringList value)
{
    QSqlQuery sqlquery;
    QVector<QStringList> result;
    QStringList result_;
    if(!sqlquery.exec(sql))
    {
        sqlquery.clear();
        qDebug()<<"query fail"<<sqlquery.lastError()<<endl;
        return result;
    }
    int j=0;
    while(sqlquery.next())
    {
        for(int i=0;i<value.size();i++)
        {
            result_<<sqlquery.value(value.at(i)).toString();
            qDebug()<<sqlquery.value(value.at(i))<<endl;
        }
        result.insert(j,result_);
        j++;
    }
    sqlquery.clear();
    return result;
}
