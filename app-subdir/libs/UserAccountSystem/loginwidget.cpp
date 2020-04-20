#include "loginwidget.h"

#include <QtWidgets>

class LoginWidgetPrivate{
public:
    LoginWidgetPrivate(QWidget *parent) : owner(parent){
        avatarLabel = new QLabel(owner);
        avatarLabel->setText(QObject::tr("Avatar"));
        avatarLabel->setObjectName("AvatarLabel");
        userBox = new QComboBox(owner);
        userBox->setEditable(true);
        userBox->setObjectName("UserBox");
        promptLabel = new QLabel(owner);
        promptLabel->setObjectName("PromptLabel");
        passwordEdit = new QLineEdit(owner);
        passwordEdit->setEchoMode(QLineEdit::PasswordEchoOnEdit);
        passwordEdit->setObjectName("PasswordEdit");
        autoLoginBox = new QCheckBox(QObject::tr("AutoLogin"), owner);
        rememberPasswordBox = new QCheckBox(QObject::tr("Remember Password"), owner);
    }
    QWidget *owner;
    QLabel *avatarLabel;
    QComboBox *userBox;
    QLabel *promptLabel;
    QLineEdit *passwordEdit;
    QCheckBox *autoLoginBox;
    QCheckBox *rememberPasswordBox;
};

LoginWidget::LoginWidget(QWidget *parent) : Dialog(parent)
  , d(new LoginWidgetPrivate(this))
{
    setTitle(tr("Login Widget"));
    setMinButtonVisible(true);
    setupUI();
    resize(300, 485);
}

LoginWidget::~LoginWidget()
{
    delete d;
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
    QPushButton *recoverPasswdButton = new QPushButton(tr("Recover Password"), this);
    recoverPasswdButton->setObjectName("FlatButton");

    QPushButton *loginButton = new QPushButton(tr("Login"), this);
    loginButton->setObjectName("LoginButton");

    QGridLayout *chooseLayout = new QGridLayout;
    chooseLayout->setContentsMargins(0, 0, 0, 0);
    chooseLayout->addWidget(d->autoLoginBox, 0, 0);
    chooseLayout->addWidget(d->rememberPasswordBox, 0, 1);
    chooseLayout->addWidget(loginButton, 1 ,0, 1, 2);
    chooseLayout->addWidget(registerButton, 2, 0);
    chooseLayout->addWidget(recoverPasswdButton, 2, 1);

    QWidget *centrawlidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centrawlidget);
    layout->addLayout(avatarLayout);
    layout->addWidget(d->userBox);
    layout->addWidget(d->promptLabel);
    layout->addWidget(d->passwordEdit);
//    layout->addWidget(loginButton);
    layout->addLayout(chooseLayout);

    setCentralWidget(centrawlidget);
}

void LoginWidget::buildConnect()
{
    //connect(d->userBox, &QComboBox::);
}
