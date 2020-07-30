#ifndef JSON_H
#define JSON_H

#include <QJsonObject>
#include <QObject>

struct JsonPrivate;
class Json : public QObject
{
    Q_OBJECT
public:
    explicit Json(const QString &jsonOrFilePath,
                  bool jsonfile = false,
                  QObject *parent = nullptr);
    ~Json();

    QString errorString() const;

    QVariant getValue(const QString &path,
                      const QJsonObject &fromNode = QJsonObject()) const;

    QStringList getStringList(const QString &path,
                              const QJsonObject &fromNode = QJsonObject()) const;

private:
    void loadJson(const QString &jsonOrFilePath, bool jsonfile = false);

    QJsonArray getJsonArray(const QString &path,
                            const QJsonObject &fromNode = QJsonObject()) const;

    QJsonValue getJsonValue(const QString &path,
                            const QJsonObject &fromNode = QJsonObject()) const;

    QScopedPointer<JsonPrivate> d;
};

#endif // JSON_H
