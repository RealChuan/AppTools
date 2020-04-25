#ifndef LOGOUTORDELETE_H
#define LOGOUTORDELETE_H

#include <controls/dialog.h>

class CurrentLoginWidgetPrivate;
class CurrentLoginWidget : public Dialog
{
    Q_OBJECT
public:
    CurrentLoginWidget(const QString &username, const QString &password, QWidget *parent = nullptr);
    ~CurrentLoginWidget();

    QString password() const;

private slots:
    void onDeleteAccount();
    void onChangePassword();

private:
    void setupUI();

    CurrentLoginWidgetPrivate *d;
};

#endif // LOGOUTORDELETE_H
