#ifndef REGISTERWIDGET_H
#define REGISTERWIDGET_H

#include <QWidget>

namespace AccountSystem {

class AccountQuery;
class RegisterWidgetPrivate;
class RegisterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RegisterWidget(AccountQuery*, QWidget *parent = nullptr);
    ~RegisterWidget();

    QString username() const;
    QString password() const;

signals:
    void registered();
    void cancel();

private slots:
    void onRegister();

private:
    void setupUI();

    QScopedPointer<RegisterWidgetPrivate> d;
};

}

#endif // REGISTERWIDGET_H
