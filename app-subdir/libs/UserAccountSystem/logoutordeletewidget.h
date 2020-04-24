#ifndef LOGOUTORDELETE_H
#define LOGOUTORDELETE_H

#include <controls/dialog.h>

class LogoutOrDeleteWidgetPrivate;
class LogoutOrDeleteWidget : public Dialog
{
    Q_OBJECT
public:
    LogoutOrDeleteWidget(const QString &username, const QString &password, QWidget *parent = nullptr);
    ~LogoutOrDeleteWidget();

private slots:
    void onDeleteAccount();

private:
    void setupUI();

    LogoutOrDeleteWidgetPrivate *d;
};

#endif // LOGOUTORDELETE_H
