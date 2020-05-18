#ifndef CHANGEPASSWD_H
#define CHANGEPASSWD_H

#include <controls/dialog.h>

class AccountQuery;
class ChangePasswdWidgetPrivate;
class ChangePasswdWidget : public Dialog
{
    Q_OBJECT
public:
    ChangePasswdWidget(AccountQuery*, const QString &,
                       const QString&, QWidget *parent = nullptr);
    ~ChangePasswdWidget();

    QString password() const;

private slots:
    void onChangePasswd();

private:
    void setupUI();

    QScopedPointer<ChangePasswdWidgetPrivate> d;
};

#endif // CHANGEPASSWD_H
