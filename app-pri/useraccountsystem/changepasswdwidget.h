#ifndef CHANGEPASSWD_H
#define CHANGEPASSWD_H

#include <QWidget>

namespace AccountSystem {

class AccountQuery;
class ChangePasswdWidgetPrivate;
class ChangePasswdWidget : public QWidget
{
    Q_OBJECT
public:
    ChangePasswdWidget(AccountQuery*, QWidget *parent = nullptr);
    ~ChangePasswdWidget();

    void setAccount(const QString &username, const QString& password);

    QString password() const;

    void clear();

signals:
    void modifyPassword();
    void cancel();

private slots:
    void onChangePasswd();

private:
    void setupUI();

    QScopedPointer<ChangePasswdWidgetPrivate> d;
};

}

#endif // CHANGEPASSWD_H
