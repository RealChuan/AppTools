#ifndef CHANGEPASSWD_H
#define CHANGEPASSWD_H

#include <controls/dialog.h>

class ChangePasswdWidgetPrivate;
class ChangePasswdWidget : public Dialog
{
    Q_OBJECT
public:
    ChangePasswdWidget(QWidget *parent = nullptr);
    ~ChangePasswdWidget();

private slots:
    void onChangePasswd();

private:
    void setupUI();

    ChangePasswdWidgetPrivate *d;
};

#endif // CHANGEPASSWD_H
