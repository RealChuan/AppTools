#include "changepasswdwidget.h"
#include "useraccountsystem.h"
#include "accountquery.h"

#include <controls/accountcontrols.h>

#include <QtWidgets>

using namespace Control;

namespace AccountSystem {

class ChangePasswdWidgetPrivate{
public:
    ChangePasswdWidgetPrivate(QWidget *parent)
        : owner(parent){
        avatarWidget = new AvatarWidget(owner);
        currentPasswordEdit = new PasswordLineEdit(owner);
        currentPasswordEdit->setPlaceholderText(
                    QObject::tr("Please enter the current account password."));
        newPasswordEdit = new PasswordLineEdit(owner);
        newPasswordEdit->setPlaceholderText(
                    QObject::tr("Please enter a new password."));
        promptLabel = new QLabel(owner);
        promptLabel->setObjectName("PromptLabel");
        passwdAgainEdit = new PasswordLineEdit(owner);
        passwdAgainEdit->setPlaceholderText(
                    QObject::tr("Please enter the new password again."));
    }
    QWidget *owner;
    AvatarWidget *avatarWidget;
    PasswordLineEdit *currentPasswordEdit;
    QLabel *promptLabel;
    PasswordLineEdit *newPasswordEdit;
    PasswordLineEdit *passwdAgainEdit;
    QString username;
    QString password;
    AccountQuery *accountQuery = nullptr;
};

ChangePasswdWidget::ChangePasswdWidget(AccountQuery *accountQuery,
                                       QWidget *parent)
    : QWidget(parent)
    , d(new ChangePasswdWidgetPrivate(this))
{
    d->accountQuery = accountQuery;
    setupUI();
}

ChangePasswdWidget::~ChangePasswdWidget()
{
}

void ChangePasswdWidget::setAccount(const QString &username, const QString &password)
{
    d->username = username;
    d->password = password;
}

QString ChangePasswdWidget::password() const
{
    return d->password;
}

void ChangePasswdWidget::clear()
{
    d->currentPasswordEdit->clear();
    d->newPasswordEdit->clear();
    d->passwdAgainEdit->clear();
}

void ChangePasswdWidget::onChangePasswd()
{
    QString currentPassword = d->currentPasswordEdit->text().trimmed();
    if(currentPassword.isEmpty()){
        d->promptLabel->setText(tr("Please enter the password!"));
        d->currentPasswordEdit->setFocus();
        return;
    }
    QString newPassword = d->newPasswordEdit->text().trimmed();
    if(newPassword.isEmpty()){
        d->promptLabel->setText(tr("Please enter the new password!"));
        d->newPasswordEdit->setFocus();
        return;
    }
    QString passwdAgain = d->passwdAgainEdit->text().trimmed();
    if(passwdAgain.isEmpty()){
        d->promptLabel->setText(tr("Please enter the password again!"));
        d->passwdAgainEdit->setFocus();
        return;
    }

    if(currentPassword != d->password){
        d->promptLabel->setText(tr("The current account %1 password is "
                                   "entered incorrectly, please re-enter!")
                                .arg(d->username));
        d->currentPasswordEdit->setFocus();
        return;
    }
    if(newPassword == d->password){
        d->promptLabel->setText("New and old passwords are the same, "
                                "please re-enter!");
        d->newPasswordEdit->setFocus();
        return;
    }
    if(newPassword != passwdAgain){
        d->promptLabel->setText("The new password is entered twice, "
                                "please re-enter!");
        d->passwdAgainEdit->setFocus();
        return;
    }
    d->promptLabel->clear();
    if(d->accountQuery->updateAccount(d->username, newPassword)){
        d->password = newPassword;
        emit modifyPassword();
        return;
    }
    d->promptLabel->setText(tr("Registration failed, please try again later!"));
}

void ChangePasswdWidget::setupUI()
{
    QPushButton *changePasswdButton = new QPushButton(tr("Changed Password"), this);
    QPushButton *cancelButton = new QPushButton(tr("Cancel"), this);
    changePasswdButton->setObjectName("BlueButton");
    cancelButton->setObjectName("GrayButton");
    connect(changePasswdButton, &QPushButton::clicked, this, &ChangePasswdWidget::onChangePasswd);
    connect(cancelButton, &QPushButton::clicked, this, &ChangePasswdWidget::cancel);
    connect(d->passwdAgainEdit, &PasswordLineEdit::returnPressed, this, &ChangePasswdWidget::onChangePasswd);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(cancelButton);
    btnLayout->addWidget(changePasswdButton);

    setObjectName("WhiteWidget");
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(d->avatarWidget);
    layout->addWidget(d->currentPasswordEdit);
    layout->addWidget(d->promptLabel);
    layout->addWidget(d->newPasswordEdit);
    layout->addWidget(d->passwdAgainEdit);
    layout->addLayout(btnLayout);
}

}
