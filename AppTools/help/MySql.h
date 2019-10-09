#ifndef MYSQL_H
#define MYSQL_H

#include <QSqlDatabase>
#include <QSqlQuery>

class MySql
{
public:
    MySql(QString ip,int port,QString databaseName,QString userName,QString passWord);
    ~MySql();

    //增删改
    void query(QString sql);
    //查
    QVector<QStringList> query(QString sql,QStringList value);

private:
    QSqlDatabase dbMySql;
    const QSqlResult* sqlResult;
};

#endif // MYSQL_H
