#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H
#include <controls/dialog.h>

class AccountQuery;
class LoginWidgetPrivate;
class LoginWidget : public Dialog
{
    Q_OBJECT
public:
    LoginWidget(AccountQuery*, const QStringList& ,QWidget *parent = nullptr);
    ~LoginWidget();

    QString username() const;
    QString password() const;
    QStringList usernameList() const;
    bool autoLogin();

private slots:
    void onLogin();
    void onRegister();

private:
    void setupUI();
    void buildConnect();

    QScopedPointer<LoginWidgetPrivate> d;
};

#endif // LOGINWIDGET_H
