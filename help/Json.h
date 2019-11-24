#ifndef JSON_H
#define JSON_H

#include <QJsonObject>
#include <QMap>
#include <QStringList>

#pragma execution_character_set("utf-8")

class Json
{
public:
    Json(QString jsonPath);
    ~Json();
    //解析json普通节点
    QJsonValue getJsonValue(QString rootName,QJsonObject fromNode=QJsonObject());
    //解析json数组
    QStringList getJsonArray(QString rootName,QJsonObject fromNode=QJsonObject());

    QJsonValue getObjectValue(QString rootName,QString nodeName,QJsonObject fromNode=QJsonObject());

    QString getString(QString rootName,QString nodeName,QJsonObject fromNode=QJsonObject());
    int getInt(QString rootName,QString nodeName,QJsonObject fromNode=QJsonObject());

private:
    bool jsonLoad;
    QJsonObject rootObj;    //根节点
};

#endif // JSON_H
