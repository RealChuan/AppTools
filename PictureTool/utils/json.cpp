#include "json.h"

#include <QJsonParseError>
#include <QFile>
#include <QDebug>
#include <QJsonArray>
#include <QRegularExpression>

class JsonPrivate{
public:
    JsonPrivate(QObject *parent) : owner(parent){}
    QObject *owner;
    bool jsonLoad = false;
    QJsonDocument jsonDoc;  // Json的文档对象
    QJsonObject rootObj;    // 根节点
    QString errorString;
};

Json::Json(const QString &jsonOrFilePath, bool jsonfile, QObject *parent)
    : QObject(parent)
    , d(new JsonPrivate(this))
{
    loadJson(jsonOrFilePath, jsonfile);
}

Json::~Json()
{

}

QString Json::errorString() const
{
    return d->errorString;
}

QVariant Json::getValue(const QString &path, const QJsonObject &fromNode) const
{
    if(!d->jsonLoad)
        return QVariant();
    return getJsonValue(path, fromNode).toVariant();
}

QStringList Json::getStringList(const QString &path, const QJsonObject &fromNode) const
{
    if(!d->jsonLoad)
        return QStringList();
    QStringList list;
    QJsonArray array = getJsonValue(path, fromNode).toArray();
    foreach(const QJsonValue &value, array)
        list.append(value.toString());

    return list;
}

void Json::loadJson(const QString &jsonOrFilePath, bool jsonfile)
{
    QByteArray json;
    if(jsonfile){
        QFile file(jsonOrFilePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
            d->errorString = QString(tr("Cannot open the file: %1")).arg(jsonOrFilePath);
            qDebug() << d->errorString;
            return;
        }
        json = file.readAll();
        file.close();
    }else
        json = jsonOrFilePath.toUtf8();

    // 解析 Json
    QJsonParseError jsonError;
    d->jsonDoc = QJsonDocument::fromJson(json, &jsonError);
    if (QJsonParseError::NoError == jsonError.error){
        d->rootObj = d->jsonDoc.object();
        d->jsonLoad = true;
    }else{
        d->errorString = QString(tr("%1\nOffset: %2")).arg(jsonError.errorString()).
                arg(jsonError.offset);
        qDebug() << d->errorString;
    }
}

QJsonArray Json::getJsonArray(const QString &path, const QJsonObject &fromNode) const
{
    // 如果根节点是数组时特殊处理
    if (("." == path || "" == path) && fromNode.isEmpty())
        return d->jsonDoc.array();

    return getJsonValue(path, fromNode).toArray();
}

QJsonValue Json::getJsonValue(const QString &path, const QJsonObject &fromNode) const
{
    QJsonObject parent = fromNode.isEmpty() ? d->rootObj : fromNode;
    QStringList names  = path.split(QRegularExpression("\\."));

    int size = names.size();
    for (int i = 0; i < size - 1; ++i) {
        if (parent.isEmpty()){
            d->errorString = QString(tr("%1 is empty!").arg(names.at(i)));
            qDebug() << d->errorString;
            return QJsonValue();
        }

        parent = parent.value(names.at(i)).toObject();
    }

    return parent.value(names.last());
}


