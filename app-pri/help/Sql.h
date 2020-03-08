#ifndef SQL_H
#define SQL_H

#include <QSqlDatabase>
#include <QSqlQuery>

class dbSql
{
public:
    dbSql();
    ~dbSql();

    void openMYSQL(QString ip,int port,QString databaseName,QString userName,QString passWord);

    void openSQLITE(QString databaseName);

    QSqlQuery query(QString sql);

    QString getString(QString sql,QString value);

private:
    QSqlDatabase dbSQL;
};

#endif // SQL_H
