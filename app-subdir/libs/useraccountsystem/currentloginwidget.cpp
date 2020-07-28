#include "currentloginwidget.h"
#include "useraccountsystem.h"
#include "accountquery.h"
#include "changepasswdwidget.h"

#include <controls/accountcontrols.h>

#include <QtWidgets>

class CurrentLoginWidgetPrivate{
public:
    CurrentLoginWidgetPrivate(QWidget *parent)
        : owner(parent){
        avatarWidget = new AvatarWidget(owner);
        accountLabel = new QLabel(owner);
        accountLabel->setObjectName("AccountLabel");
        passwordEdit = new PasswordLineEdit(owner);
        passwordEdit->setPlaceholderText(
                    QObject::tr("Please enter password and click delete again!"));
        promptLabel = new QLabel(owner);
        promptLabel->setObjectName("PromptLabel");
    }
    QWidget *owner;
    AvatarWidget *avatarWidget;
    QLabel *accountLabel;
    QLabel *promptLabel;
    PasswordLineEdit *passwordEdit;
    QString username;
    QString password;
    AccountQuery *accountQuery = nullptr;
};

CurrentLoginWidget::CurrentLoginWidget(AccountQuery *accountQuery,
                                       const QString &username,
                                       const QString &password,
                                       QWidget *parent)
    : Dialog(parent)
    , d(new CurrentLoginWidgetPrivate(this))
{
    d->username = username;
    d->password = password;
    d->accountQuery = accountQuery;
    setTitle(tr("Current Login Widget"));
    setMinButtonVisible(true);
    setupUI();
    resize(300, 485);
}

CurrentLoginWidget::~CurrentLoginWidget()
{
}

QString CurrentLoginWidget::password() const
{
    return d->password;
}

void CurrentLoginWidget::onDeleteAccount()
{
    d->promptLabel->clear();
    if(d->passwordEdit->isVisible()){
        if(d->passwordEdit->text() == d->password
                && d->accountQuery->deleteAccount(d->username)){
            reject();
        }else
            d->promptLabel->setText(tr("Wrong password, please re-enter!"));
        return;
    }
    d->passwordEdit->show();
    d->passwordEdit->setFocus();
}

void CurrentLoginWidget::onChangePassword()
{
    ChangePasswdWidget dialog(d->accountQuery, d->username, d->password, this);
    if(dialog.exec() == ChangePasswdWidget::Accepted)
        d->password = dialog.password();
}

void CurrentLoginWidget::setupUI()
{
    QPushButton *logoutButton = new QPushButton(tr("Log Out"), this);
    QPushButton *changePasswdButton = new QPushButton(tr("Change Password"), this);
    QPushButton *deleteAccountButton = new QPushButton(tr("Delete Account"), this);
    logoutButton->setObjectName("BlueButton");
    changePasswdButton->setObjectName("BlueButton");
    deleteAccountButton->setObjectName("GrayButton");
    connect(logoutButton, &QPushButton::clicked, this, &CurrentLoginWidget::accepted);
    connect(changePasswdButton, &QPushButton::clicked, this, &CurrentLoginWidget::onChangePassword);
    connect(deleteAccountButton, &QPushButton::clicked, this, &CurrentLoginWidget::onDeleteAccount);
    d->accountLabel->setText(tr("Current online account: %1.").arg(d->username));
    d->passwordEdit->hide();

    QWidget *widget = new QWidget(this);
    widget->setObjectName("WhiteWidget");
    QGridLayout *layout = new QGridLayout(widget);
    layout->addWidget(d->avatarWidget, 0, 0, 1, 2);
    layout->addWidget(d->accountLabel, 1, 0, 1, 2);
    layout->addWidget(d->promptLabel, 2, 0, 1, 2);
    layout->addWidget(d->passwordEdit, 3, 0, 1, 2);
    layout->addWidget(changePasswdButton, 4, 0, 1, 1);
    layout->addWidget(deleteAccountButton, 4, 1, 1, 1);
    layout->addWidget(logoutButton, 5, 0, 1, 2);
    setCentralWidget(widget);
}
