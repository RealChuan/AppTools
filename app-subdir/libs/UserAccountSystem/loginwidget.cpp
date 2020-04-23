#include "changepasswdwidget.h"
#include "loginwidget.h"
#include "registerwidget.h"

#include <controls/editcombobox.h>

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

LoginWidget::LoginWidget(QWidget *parent) : Dialog(parent)
  , d(new LoginWidgetPrivate(this))
{
    setObjectName("LoginWidget");
    setTitle(tr("Login Widget"));
    setMinButtonVisible(true);
    setupUI();
    resize(300, 485);
}

LoginWidget::~LoginWidget()
{
    delete d;
}

void LoginWidget::onLogin()
{

}

void LoginWidget::onRegister()
{
    RegisterWidget regist(this);
    regist.exec();
}

void LoginWidget::onChangePasswd()
{
    ChangePasswdWidget changePasswd(this);
    changePasswd.exec();
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
    QPushButton *changePasswdButton = new QPushButton(tr("Change Password"), this);
    changePasswdButton->setObjectName("FlatButton");
    connect(changePasswdButton, &QPushButton::clicked, this, &LoginWidget::onChangePasswd);

    QPushButton *loginButton = new QPushButton(tr("Login"), this);
    loginButton->setObjectName("LoginButton");
    connect(loginButton, &QPushButton::clicked, this, &LoginWidget::onLogin);

    QGridLayout *chooseLayout = new QGridLayout;
    chooseLayout->setContentsMargins(0, 0, 0, 0);
    chooseLayout->addWidget(d->autoLoginBox, 0, 0);
    chooseLayout->addWidget(d->rememberPasswordBox, 0, 1);
    chooseLayout->addWidget(loginButton, 1 ,0, 1, 2);
    chooseLayout->addWidget(registerButton, 2, 0);
    chooseLayout->addWidget(changePasswdButton, 2, 1);

    QWidget *centrawlidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centrawlidget);
    layout->addLayout(avatarLayout);
    layout->addWidget(d->usernameBox);
    layout->addWidget(d->promptLabel);
    layout->addWidget(d->passwordEdit);
    layout->addLayout(chooseLayout);

    setCentralWidget(centrawlidget);
}

void LoginWidget::buildConnect()
{
    connect(d->passwordEdit, &QLineEdit::returnPressed, this, &LoginWidget::onLogin);
}
