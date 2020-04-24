#include "changepasswdwidget.h"

#include <QtWidgets>

class ChangePasswdWidgetPrivate{
public:
    ChangePasswdWidgetPrivate(QWidget *parent) : owner(parent){
        avatarLabel = new QLabel(owner);
        avatarLabel->setText(QObject::tr("Avatar"));
        avatarLabel->setObjectName("AvatarLabel");
        usernameEdit = new QLineEdit(owner);
        usernameEdit->setObjectName("UsernameEdit");    
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
    QLabel *promptLabel;
    QLineEdit *passwordEdit;
    QLineEdit *passwdAgainEdit;
};

ChangePasswdWidget::ChangePasswdWidget(QWidget *parent) : Dialog(parent)
  , d(new ChangePasswdWidgetPrivate(this))
{
    setTitle(tr("ChangePasswd Widget"));
    setMinButtonVisible(true);
    setupUI();
    resize(300, 485);
}

ChangePasswdWidget::~ChangePasswdWidget()
{
    delete d;
}

void ChangePasswdWidget::onChangePasswd()
{

}

void ChangePasswdWidget::setupUI()
{
    QGridLayout *avatarLayout = new QGridLayout;
    avatarLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), 0, 2);
    avatarLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), 2, 2);
    avatarLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum), 1, 1);
    avatarLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum), 1, 3);
    avatarLayout->addWidget(d->avatarLabel, 1, 2);

    QPushButton *changePasswdButton = new QPushButton(tr("Changed Password"), this);
    connect(changePasswdButton, &QPushButton::clicked, this, &ChangePasswdWidget::onChangePasswd);
    QPushButton *cancelButton = new QPushButton(tr("Cancel"), this);
    connect(cancelButton, &QPushButton::clicked, this, &ChangePasswdWidget::reject);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(cancelButton);
    btnLayout->addWidget(changePasswdButton);

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
