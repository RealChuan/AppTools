#include "loginwidget.h"
#include "registerwidget.h"
#include "useraccountsystem.h"
#include "accountquery.h"

#include <controls/editcombobox.h>
#include <controls/messbox.h>

#include <QtWidgets>

class LoginWidgetPrivate{
public:
    LoginWidgetPrivate(QWidget *parent) : owner(parent){
        avatarLabel = new QLabel(owner);
        avatarLabel->setText(QObject::tr("Avatar"));
        avatarLabel->setObjectName("AvatarLabel");
        usernameBox = new EditComboBox(owner);
        promptLabel = new QLabel(owner);
        promptLabel->setObjectName("PromptLabel");
        passwordEdit = new QLineEdit(owner);
        passwordEdit->setEchoMode(QLineEdit::PasswordEchoOnEdit);
        autoLoginBox = new QCheckBox(QObject::tr("AutoLogin"), owner);
        rememberPasswordBox = new QCheckBox(QObject::tr("Remember Password"), owner);
    }
    QWidget *owner;
    QLabel *avatarLabel;
    EditComboBox *usernameBox;
    QLabel *promptLabel;
    QLineEdit *passwordEdit;
    QCheckBox *autoLoginBox;
    QCheckBox *rememberPasswordBox;
};

LoginWidget::LoginWidget(const QStringList &usernameList, QWidget *parent) : Dialog(parent)
  , d(new LoginWidgetPrivate(this))
{
    setObjectName("LoginWidget");
    setTitle(tr("Login Widget"));
    setMinButtonVisible(true);
    setupUI();
    buildConnect();
    for(const QString& username: usernameList){
        d->usernameBox->addAccount(username);
    }
    resize(300, 485);
}

LoginWidget::~LoginWidget()
{
    delete d;
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

void LoginWidget::onLogin()
{
    QString username = d->usernameBox->currentText().trimmed();
    if(username.isEmpty()){
        MessBox::Warning(this, tr("Username is empty, please enter username!"), MessBox::CloseButton);
        d->usernameBox->setFocus();
        return;
    }
    QString password = d->passwordEdit->text().trimmed();
    if(password.isEmpty()){
        MessBox::Warning(this, tr("Password is empty, please enter password!"), MessBox::CloseButton);
        d->passwordEdit->setFocus();
        return;
    }

    AccountQuery * query =  UserAccountSystem::accountQuery();
    if(query->checkAccount(username, password)){
        d->usernameBox->addAccount(username);
        accept();
        return;
    }
    d->promptLabel->setText(tr("Incorrect account or password!"));
}

void LoginWidget::onRegister()
{
    RegisterWidget regist(this);
    if(regist.exec() == RegisterWidget::Accepted){
        d->usernameBox->addAccount(regist.username());
        d->passwordEdit->setText(regist.password());
        accept();
    }
}

void LoginWidget::setupUI()
{
    QGridLayout *avatarLayout = new QGridLayout;
    avatarLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), 0, 2);
    avatarLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), 2, 2);
    avatarLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum), 1, 1);
    avatarLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum), 1, 3);
    avatarLayout->addWidget(d->avatarLabel, 1, 2);

    QPushButton *registerButton = new QPushButton(tr("Registered"), this);
    registerButton->setObjectName("FlatButton");
    connect(registerButton, &QPushButton::clicked, this, &LoginWidget::onRegister);

    QPushButton *loginButton = new QPushButton(tr("Login"), this);
    loginButton->setObjectName("LoginButton");
    connect(loginButton, &QPushButton::clicked, this, &LoginWidget::onLogin);

    QHBoxLayout *chooseLayout = new QHBoxLayout;
    chooseLayout->setContentsMargins(0, 0, 0, 0);
    chooseLayout->addWidget(d->autoLoginBox);
    chooseLayout->addWidget(d->rememberPasswordBox);
    chooseLayout->addWidget(registerButton);

    QWidget *centrawlidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centrawlidget);
    layout->addLayout(avatarLayout);
    layout->addWidget(d->usernameBox);
    layout->addWidget(d->promptLabel);
    layout->addWidget(d->passwordEdit);
    layout->addLayout(chooseLayout);
    layout->addWidget(loginButton);

    setCentralWidget(centrawlidget);
}

void LoginWidget::buildConnect()
{
    connect(d->passwordEdit, &QLineEdit::returnPressed, this, &LoginWidget::onLogin);
}
