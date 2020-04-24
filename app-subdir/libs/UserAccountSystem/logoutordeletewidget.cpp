#include "logoutordeletewidget.h"
#include "useraccountsystem.h"
#include "accountquery.h"

#include <QtWidgets>

class LogoutOrDeleteWidgetPrivate{
public:
    LogoutOrDeleteWidgetPrivate(QWidget *parent) : owner(parent){
        avatarLabel = new QLabel(owner);
        avatarLabel->setText(QObject::tr("Avatar"));
        avatarLabel->setObjectName("AvatarLabel");
        accountLabel = new QLabel(owner);
        passwordEdit = new QLineEdit(owner);
        passwordEdit->setPlaceholderText(QObject::tr("Please enter password and click delete again!"));
        promptLabel = new QLabel(owner);
        promptLabel->setObjectName("PromptLabel");
    }
    QWidget *owner;
    QLabel *accountLabel;
    QLabel *avatarLabel;
    QLabel *promptLabel;
    QLineEdit *passwordEdit;
    QString username;
    QString password;
};

LogoutOrDeleteWidget::LogoutOrDeleteWidget(const QString &username, const QString &password, QWidget *parent)
    : Dialog(parent)
    , d(new LogoutOrDeleteWidgetPrivate(this))
{
    d->username = username;
    d->password = password;
    setTitle(tr("ChangePasswd Widget"));
    setMinButtonVisible(true);
    setupUI();
    resize(300, 485);
}

LogoutOrDeleteWidget::~LogoutOrDeleteWidget()
{
    delete d;
}

void LogoutOrDeleteWidget::onDeleteAccount()
{
    if(d->passwordEdit->isVisible()){
        if(d->passwordEdit->text() == d->password){
            AccountQuery * query =  UserAccountSystem::accountQuery();
            if(query->deleteAccount(d->username))
                reject();
        }else
            d->promptLabel->setText(tr("Wrong password, please re-enter!"));
        return;
    }
    d->passwordEdit->show();
    d->passwordEdit->setFocus();
}

void LogoutOrDeleteWidget::setupUI()
{
    QGridLayout *avatarLayout = new QGridLayout;
    avatarLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), 0, 2);
    avatarLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), 2, 2);
    avatarLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum), 1, 1);
    avatarLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum), 1, 3);
    avatarLayout->addWidget(d->avatarLabel, 1, 2);

    QPushButton *logoutButton = new QPushButton(tr("Log Out"), this);
    QPushButton *deleteAccountButton = new QPushButton(tr("Delete Account"), this);
    connect(logoutButton, &QPushButton::clicked, this, &LogoutOrDeleteWidget::accepted);
    connect(deleteAccountButton, &QPushButton::clicked, this, &LogoutOrDeleteWidget::onDeleteAccount);

    d->accountLabel->setText(tr("Current online account: %1.").arg(d->username));
    d->passwordEdit->hide();

    QWidget *widget = new QWidget(this);
    QGridLayout *layout = new QGridLayout(widget);
    layout->addLayout(avatarLayout, 0, 0, 1, 2);
    layout->addWidget(d->accountLabel, 1, 0, 1, 2);
    layout->addWidget(d->promptLabel, 2, 0, 1, 2);
    layout->addWidget(d->passwordEdit, 3, 0, 1, 2);
    layout->addWidget(logoutButton, 4, 0, 1, 1);
    layout->addWidget(deleteAccountButton, 4, 1, 1, 1);
    setCentralWidget(widget);
}
