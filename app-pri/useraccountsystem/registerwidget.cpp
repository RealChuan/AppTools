#include "registerwidget.h"
#include "useraccountsystem.h"
#include "accountquery.h"

#include <controls/accountcontrols.h>

#include <QtWidgets>

class RegisterWidgetPrivate{
public:
    RegisterWidgetPrivate(QWidget *parent)
        : owner(parent){
        avatarWidget = new AvatarWidget(owner);
        usernameEdit = new QLineEdit(owner);
        usernameEdit->setObjectName("usernameEdit");
        usernameEdit->setPlaceholderText(QObject::tr("Please enter user name."));
        passwordEdit = new PasswordLineEdit(owner);
        passwordEdit->setPlaceholderText(QObject::tr("Please enter the password."));
        promptLabel = new QLabel(owner);
        promptLabel->setObjectName("PromptLabel");
        passwdAgainEdit = new PasswordLineEdit(owner);
        passwdAgainEdit->setPlaceholderText(QObject::tr("Please enter the password again."));
    }
    QWidget *owner;
    AvatarWidget *avatarWidget;
    QLineEdit *usernameEdit;
    PasswordLineEdit *passwordEdit;
    QLabel *promptLabel;
    PasswordLineEdit *passwdAgainEdit;
    AccountQuery *accountQuery = nullptr;
};

RegisterWidget::RegisterWidget(AccountQuery *accountQuery, QWidget *parent)
    : Dialog(parent)
    , d(new RegisterWidgetPrivate(this))
{
    setTitle(tr("Register Widget"));
    d->accountQuery = accountQuery;
    setMinButtonVisible(true);
    setupUI();
    resize(300, 485);
}

RegisterWidget::~RegisterWidget()
{
}

QString RegisterWidget::username() const
{
    return d->usernameEdit->text();
}

QString RegisterWidget::password() const
{
    return d->passwordEdit->text();
}

void RegisterWidget::onRegister()
{
    d->promptLabel->clear();
    QString username = d->usernameEdit->text().trimmed();
    if(username.isEmpty()){
        d->promptLabel->setText(tr("Please enter username!"));
        d->usernameEdit->setFocus();
        return;
    }
    QString password = d->passwordEdit->text().trimmed();
    if(password.isEmpty()){
        d->promptLabel->setText(tr("Please enter password!"));
        d->passwordEdit->setFocus();
        return;
    }
    QString passwdAgain = d->passwdAgainEdit->text().trimmed();
    if(passwdAgain.isEmpty()){
        d->promptLabel->setText(tr("Please enter password again!"));
        d->passwdAgainEdit->setFocus();
        return;
    }
    if(password != passwdAgain){
        d->promptLabel->setText(tr("Two passwords are different, "
                                   "please enter password again!"));
        d->passwdAgainEdit->setFocus();
        return;
    }
    if(d->accountQuery->contains(username)){
        d->promptLabel->setText(tr("An account with the same name already exists, "
                                   "please modify the account name!"));
        d->usernameEdit->setFocus();
        return;
    }
    if(d->accountQuery->addAccount(username, password)){
        accept();
        return;
    }
    d->promptLabel->setText(tr("Registration failed, please try again later!"));
}

void RegisterWidget::setupUI()
{
    QPushButton *registerButton = new QPushButton(tr("Registered"), this);
    QPushButton *cancelButton = new QPushButton(tr("Cancel"), this);
    registerButton->setObjectName("BlueButton");
    cancelButton->setObjectName("GrayButton");
    connect(registerButton, &QPushButton::clicked, this, &RegisterWidget::onRegister);
    connect(cancelButton, &QPushButton::clicked, this, &RegisterWidget::reject);
    connect(d->passwdAgainEdit, &PasswordLineEdit::returnPressed,
            this, &RegisterWidget::onRegister);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(cancelButton);
    btnLayout->addWidget(registerButton);

    QWidget *widget = new QWidget(this);
    widget->setObjectName("WhiteWidget");
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->addWidget(d->avatarWidget);
    layout->addWidget(d->usernameEdit);
    layout->addWidget(d->passwordEdit);
    layout->addWidget(d->promptLabel);
    layout->addWidget(d->passwdAgainEdit);
    layout->addLayout(btnLayout);

    setCentralWidget(widget);
}
