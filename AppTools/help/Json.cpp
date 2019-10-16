#include "Json.h"

#include <QFile>
#include <QDebug>
#include <QJsonParseError>
#include <QJsonArray>

Json::Json(QString jsonPath)
    :jsonLoad(false)
{
    QString jsonError;
    QFile file(jsonPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        jsonError=QString("Cannot open the file: %1").arg(jsonPath);
        qDebug()<<jsonError;
        qDebug()<<"could't open file json";
        return;
    }
    QByteArray json = file.readAll();
    file.close();

    // 解析 Json
    QJsonParseError error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(json, &error);
    if (QJsonParseError::NoError != error.error)
    {
        jsonError = QString("%1\nOffset: %2").arg(error.errorString()).arg(error.offset);
        qDebug() << jsonError;
        return;
    }
    rootObj = jsonDoc.object();
    jsonLoad=true;
}

Json::~Json()
{

}

QJsonValue Json::getJsonValue(QString rootName,QJsonObject fromNode)
{
    if(!jsonLoad)
        return 0;
    QJsonValue subObj;
    if(fromNode.isEmpty()&&rootObj.contains(rootName))
    {
        subObj = rootObj.value(rootName);
    }
    else if(!fromNode.isEmpty()&&fromNode.contains(rootName))
    {
        subObj =fromNode.value(rootName);
    }
    else
    {
        QString jsonError;
        jsonError = QString("can not find %1").arg(rootName);
        qDebug() << jsonError;
        return 0;
    }
    return subObj;
}

QStringList Json::getJsonArray(QString rootName,QJsonObject fromNode)
{
    QStringList jsonArray;
    QJsonArray subArray=getJsonValue(rootName,fromNode).toArray();
    for(int i = 0; i< subArray.size(); i++)
    {
        jsonArray << subArray.at(i).toString();
    }
    return jsonArray;
}

QJsonValue Json::getObjectValue(QString rootName, QString nodeName, QJsonObject fromNode)
{
    QJsonObject subObject=getJsonValue(rootName,fromNode).toObject();
    for(int i=0;i<subObject.size();i++)
    {
        if(subObject.contains(nodeName))
            return subObject.value(nodeName);
    }
    return 0;
}

QString Json::getString(QString rootName, QString nodeName, QJsonObject fromNode)
{
    QJsonValue value=getObjectValue(rootName,nodeName,fromNode);
    if(value.isNull())
    {
        QString jsonError;
        jsonError = QString("can not find %1").arg(rootName);
        qDebug() << jsonError;
    }
    return value.toString();
}

int Json::getInt(QString rootName, QString nodeName, QJsonObject fromNode)
{
    QJsonValue value=getObjectValue(rootName,nodeName,fromNode);
    if(value.isNull())
    {
        QString jsonError;
        jsonError = QString("can not find %1").arg(rootName);
        qDebug() << jsonError;
    }
    return value.toInt();
}
