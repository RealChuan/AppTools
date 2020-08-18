#ifndef LOGOUTORDELETE_H
#define LOGOUTORDELETE_H

#include <controls/dialog.h>

using namespace Control;

namespace AccountSystem {

class AccountQuery;
class CurrentLoginWidgetPrivate;
class CurrentLoginWidget : public Dialog
{
    Q_OBJECT
public:
    CurrentLoginWidget(AccountQuery*,
                       const QString &,
                       const QString &,
                       QWidget *parent = nullptr);
    ~CurrentLoginWidget();

    QString password() const;

private slots:
    void onDeleteAccount();
    void onChangePassword();

private:
    void setupUI();

    QScopedPointer<CurrentLoginWidgetPrivate> d;
};

}

#endif // LOGOUTORDELETE_H
