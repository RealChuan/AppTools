#include "currentloginwidget.h"
#include "useraccountsystem.h"
#include "accountquery.h"
#include "changepasswdwidget.h"

#include <controls/accountcontrols.h>

#include <QtWidgets>

using namespace Control;

namespace AccountSystem {

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
                                       QWidget *parent)
    : QWidget(parent)
    , d(new CurrentLoginWidgetPrivate(this))
{
    d->accountQuery = accountQuery;
    setupUI();
}

CurrentLoginWidget::~CurrentLoginWidget()
{
}

void CurrentLoginWidget::setAccount(const QString &username, const QString &password)
{
    d->username = username;
    d->password = password;
    d->accountLabel->setText(tr("Current online account: %1.").arg(d->username));
    d->passwordEdit->hide();
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
            emit deleteAccount();
        }else
            d->promptLabel->setText(tr("Wrong password, please re-enter!"));
        return;
    }
    d->passwordEdit->show();
    d->passwordEdit->setFocus();
}

void CurrentLoginWidget::setupUI()
{
    QPushButton *logoutButton = new QPushButton(tr("Log Out"), this);
    QPushButton *changePasswdButton = new QPushButton(tr("Change Password"), this);
    QPushButton *deleteAccountButton = new QPushButton(tr("Delete Account"), this);
    logoutButton->setObjectName("BlueButton");
    changePasswdButton->setObjectName("BlueButton");
    deleteAccountButton->setObjectName("GrayButton");
    connect(logoutButton, &QPushButton::clicked, this, &CurrentLoginWidget::logout);
    connect(changePasswdButton, &QPushButton::clicked, this, &CurrentLoginWidget::changePassword);
    connect(deleteAccountButton, &QPushButton::clicked, this, &CurrentLoginWidget::onDeleteAccount);

    setObjectName("WhiteWidget");
    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(d->avatarWidget, 0, 0, 1, 2);
    layout->addWidget(d->accountLabel, 1, 0, 1, 2);
    layout->addWidget(d->promptLabel, 2, 0, 1, 2);
    layout->addWidget(d->passwordEdit, 3, 0, 1, 2);
    layout->addWidget(changePasswdButton, 4, 0, 1, 1);
    layout->addWidget(deleteAccountButton, 4, 1, 1, 1);
    layout->addWidget(logoutButton, 5, 0, 1, 2);
}

}
