#ifndef CHANGEPASSWD_H
#define CHANGEPASSWD_H

#include <controls/dialog.h>

class ChangePasswdWidgetPrivate;
class ChangePasswdWidget : public Dialog
{
    Q_OBJECT
public:
    ChangePasswdWidget(const QString &username, const QString& password, QWidget *parent = nullptr);
    ~ChangePasswdWidget();

    QString password() const;

private slots:
    void onChangePasswd();

private:
    void setupUI();

    ChangePasswdWidgetPrivate *d;
};

#endif // CHANGEPASSWD_H
