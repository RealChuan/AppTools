#include "loginwidget.h"
#include "registerwidget.h"
#include "useraccountsystem.h"
#include "accountquery.h"

#include <controls/accountcontrols.h>

#include <QtWidgets>

using namespace Control;

namespace AccountSystem {

class LoginWidgetPrivate{
public:
    LoginWidgetPrivate(QWidget *parent)
        : owner(parent){
        avatarWidget = new AvatarWidget(owner);
        usernameBox = new EditComboBox(owner);
        promptLabel = new QLabel(owner);
        promptLabel->setObjectName("PromptLabel");
        passwordEdit = new PasswordLineEdit(owner);
        autoLoginBox = new QCheckBox(QObject::tr("AutoLogin"), owner);
    }
    QWidget *owner;
    AvatarWidget *avatarWidget;
    EditComboBox *usernameBox;
    QLabel *promptLabel;
    PasswordLineEdit *passwordEdit;
    QCheckBox *autoLoginBox;
    AccountQuery *accountQuery = nullptr;
};

LoginWidget::LoginWidget(AccountQuery *accountQuery,
                         QWidget *parent)
    : QWidget(parent)
    , d(new LoginWidgetPrivate(this))
{
    setObjectName("LoginWidget");
    d->accountQuery = accountQuery;
    setupUI();
    buildConnect();
}

LoginWidget::~LoginWidget()
{
}

void LoginWidget::setNameList(const QStringList &usernameList)
{
    for(const QString& username: usernameList)
        d->usernameBox->addAccount(username);
}

QString LoginWidget::username() const
{
    return d->usernameBox->currentText().trimmed();
}

QString LoginWidget::password() const
{
    return d->passwordEdit->text().trimmed();
}

QStringList LoginWidget::usernameList() const
{
    return d->usernameBox->accountList();
}

bool LoginWidget::autoLogin()
{
    return d->autoLoginBox->isChecked();
}

void LoginWidget::clear()
{
    d->passwordEdit->clear();
    d->autoLoginBox->setChecked(false);
}

void LoginWidget::onLogin()
{
    d->promptLabel->clear();
    QString username = d->usernameBox->currentText().trimmed();
    if(username.isEmpty()){
        d->promptLabel->setText(tr("Please enter username!"));
        d->usernameBox->setFocus();
        return;
    }
    QString password = d->passwordEdit->text().trimmed();
    if(password.isEmpty()){
        d->promptLabel->setText(tr("Please enter password!"));
        d->passwordEdit->setFocus();
        return;
    }

    if(d->accountQuery->checkAccount(username, password)){
        if(!d->usernameBox->accountList().contains(username))
            d->usernameBox->addAccount(username);
        emit complete();
        return;
    }
    d->promptLabel->setText(tr("Incorrect account or password!"));
}

void LoginWidget::setupUI()
{
    QPushButton *registerButton = new QPushButton(tr("Registered"), this);
    registerButton->setObjectName("FlatButton");
    connect(registerButton, &QPushButton::clicked, this, &LoginWidget::registered);

    QPushButton *loginButton = new QPushButton(tr("Login"), this);
    loginButton->setObjectName("BlueButton");
    connect(loginButton, &QPushButton::clicked, this, &LoginWidget::onLogin);

    QHBoxLayout *chooseLayout = new QHBoxLayout;
    chooseLayout->setContentsMargins(0, 0, 0, 0);
    chooseLayout->addWidget(d->autoLoginBox);
    chooseLayout->addWidget(registerButton);

    setObjectName("WhiteWidget");
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(d->avatarWidget);
    layout->addWidget(d->usernameBox);
    layout->addWidget(d->promptLabel);
    layout->addWidget(d->passwordEdit);
    layout->addLayout(chooseLayout);
    layout->addWidget(loginButton);
}

void LoginWidget::buildConnect()
{
    connect(d->passwordEdit, &PasswordLineEdit::returnPressed, this, &LoginWidget::onLogin);
}

}
