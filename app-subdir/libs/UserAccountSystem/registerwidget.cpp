#include "registerwidget.h"
#include "useraccountsystem.h"
#include "accountquery.h"

#include <QtWidgets>

#include <controls/messbox.h>

class RegisterWidgetPrivate{
public:
    RegisterWidgetPrivate(QWidget *parent) : owner(parent){
        avatarLabel = new QLabel(owner);
        avatarLabel->setText(QObject::tr("Avatar"));
        avatarLabel->setObjectName("AvatarLabel");
        usernameEdit = new QLineEdit(owner);
        passwordEdit = new QLineEdit(owner);
        passwordEdit->setEchoMode(QLineEdit::PasswordEchoOnEdit);
        promptLabel = new QLabel(owner);
        promptLabel->setObjectName("PromptLabel");
        passwdAgainEdit = new QLineEdit(owner);
        passwdAgainEdit->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    }
    QWidget *owner;
    QLabel *avatarLabel;
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QLabel *promptLabel;
    QLineEdit *passwdAgainEdit;
};

RegisterWidget::RegisterWidget(QWidget *parent) : Dialog(parent)
  , d(new RegisterWidgetPrivate(this))
{
    setTitle(tr("Register Widget"));
    setMinButtonVisible(true);
    setupUI();
    resize(300, 485);
}

RegisterWidget::~RegisterWidget()
{
    delete d;
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
    QString username = d->usernameEdit->text().trimmed();
    if(username.isEmpty()){
        MessBox::Warning(this, tr("Username is empty, please enter username!"), MessBox::CloseButton);
        d->usernameEdit->setFocus();
        return;
    }
    QString password = d->passwordEdit->text().trimmed();
    if(password.isEmpty()){
        MessBox::Warning(this, tr("Password is empty, please enter password!"), MessBox::CloseButton);
        d->passwordEdit->setFocus();
        return;
    }
    QString passwdAgain = d->passwdAgainEdit->text().trimmed();
    if(passwdAgain.isEmpty()){
        MessBox::Warning(this, tr("please enter password again!"), MessBox::CloseButton);
        d->passwdAgainEdit->setFocus();
        return;
    }
    if(password != passwdAgain){
        MessBox::Warning(this, tr("Two passwords are different, please enter password again!"), MessBox::CloseButton);
        d->passwdAgainEdit->setFocus();
        return;
    }
    AccountQuery * query =  UserAccountSystem::accountQuery();
    if(query->contains(username)){
        MessBox::Warning(this, tr("An account with the same name already exists, please modify the account name!"), MessBox::CloseButton);
        d->usernameEdit->setFocus();
        return;
    }
    if(query->addAccount(username, password)){
        accept();
        return;
    }
    d->promptLabel->setText(tr("Registration failed, please try again later!"));
}

void RegisterWidget::setupUI()
{
    QGridLayout *avatarLayout = new QGridLayout;
    avatarLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), 0, 2);
    avatarLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), 2, 2);
    avatarLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum), 1, 1);
    avatarLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum), 1, 3);
    avatarLayout->addWidget(d->avatarLabel, 1, 2);

    QPushButton *registerButton = new QPushButton(tr("Registered"), this);
    connect(registerButton, &QPushButton::clicked, this, &RegisterWidget::onRegister);
    QPushButton *cancelButton = new QPushButton(tr("Cancel"), this);
    connect(cancelButton, &QPushButton::clicked, this, &RegisterWidget::reject);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(cancelButton);
    btnLayout->addWidget(registerButton);

    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->addLayout(avatarLayout);
    layout->addWidget(d->usernameEdit);
    layout->addWidget(d->passwordEdit);
    layout->addWidget(d->promptLabel);
    layout->addWidget(d->passwdAgainEdit);
    layout->addLayout(btnLayout);

    setCentralWidget(widget);
}
