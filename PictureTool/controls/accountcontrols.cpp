#include "accountcontrols.h"
#include "messbox.h"

#include <QtWidgets>

class AccountItemPrivate{
public:
    AccountItemPrivate(QWidget *parent)
        : owner(parent){
        avatarLabel = new QLabel("avatar", owner);
        usernameLabel = new QLabel("username", owner);
        usernameLabel->setObjectName("UsernameLabel");
    }
    QWidget *owner;
    QLabel *avatarLabel;
    QLabel *usernameLabel;
    bool mousePressed = false;
};

AccountItemWidget::AccountItemWidget(QWidget *parent)
    : QWidget(parent)
    , d(new AccountItemPrivate(this))
{
    setupUI();
}

AccountItemWidget::~AccountItemWidget()
{
}

void AccountItemWidget::setAvatar(const QString &image)
{
    QPixmap p(image);
    d->avatarLabel->setPixmap(p);
}

void AccountItemWidget::setUsername(const QString &username)
{
    d->usernameLabel->setText(username);
}

QString AccountItemWidget::getAccount() const
{
    return d->usernameLabel->text();
}

void AccountItemWidget::mousePressEvent(QMouseEvent *event)
{

    if(event->button() == Qt::LeftButton)
        d->mousePressed = true;
}

void AccountItemWidget::mouseReleaseEvent(QMouseEvent *)
{
    if(d->mousePressed){
        emit showAccount(d->usernameLabel->text());
        d->mousePressed = false;
    }
}

void AccountItemWidget::onRemove()
{
    emit remove(d->usernameLabel->text());
}

void AccountItemWidget::setupUI()
{
    QToolButton *deleteButton = new QToolButton(this);
    deleteButton->setObjectName("deleteButton");
    connect(deleteButton, &QPushButton::clicked, this, &AccountItemWidget::onRemove);

    d->avatarLabel->hide();
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(d->avatarLabel);
    layout->addWidget(d->usernameLabel);
    layout->addStretch();
    layout->addWidget(deleteButton);
    layout->setContentsMargins(5, 5, 5, 5);
    layout->setSpacing(5);
}

/*
 *
 * */

class EditComboBoxPrivate{
public:
    EditComboBoxPrivate(QWidget *parent)
        : owner(parent){
        listWidget = new QListWidget(owner);
    }
    QWidget *owner;
    QListWidget *listWidget;
};

EditComboBox::EditComboBox(QWidget *parent)
    : QComboBox(parent)
    , d(new EditComboBoxPrivate(this))
{
    setMaxVisibleItems(3);
    setInsertPolicy(QComboBox::NoInsert);
    setModel(d->listWidget->model());
    setView(d->listWidget);
    setEditable(true);
}

EditComboBox::~EditComboBox()
{
    delete d;
}

void EditComboBox::addAccount(const QString &username)
{
    AccountItemWidget *accountItem = new AccountItemWidget(this);
    accountItem->setUsername(username);
    connect(accountItem, &AccountItemWidget::showAccount, this, &EditComboBox::onShowText);
    connect(accountItem, &AccountItemWidget::remove, this, &EditComboBox::onDeleteItem);
    QListWidgetItem *item = new QListWidgetItem(d->listWidget);
    d->listWidget->setItemWidget(item, accountItem);
    onShowText(username);
}

QStringList EditComboBox::accountList() const
{
    QStringList list;
    for(int i=0; i<d->listWidget->count(); i++){
        QListWidgetItem *item = d->listWidget->item(i);
        AccountItemWidget *accountItem =
                qobject_cast<AccountItemWidget*>(d->listWidget->itemWidget(item));
        list.append(accountItem->getAccount());
    }
    return list;
}

void EditComboBox::onShowText(const QString &text)
{
    setEditText(text);
    hidePopup();
}

void EditComboBox::onDeleteItem(const QString &text)
{
    hidePopup();
    Dialog::ExecFlags flag = MessBox::Warning(this,
                                              tr("Confirm to delete the current account?"),
                                              MessBox::YesAndNoButton);
    showPopup();
    if(flag == Dialog::Rejected)
        return;

    for(int i=0; i<d->listWidget->count(); i++){
        QListWidgetItem *item = d->listWidget->item(i);
        AccountItemWidget *accountItem =
                qobject_cast<AccountItemWidget*>(d->listWidget->itemWidget(item));
        if(accountItem && accountItem->getAccount() == text){
            delete d->listWidget->takeItem(i);
            break;
        }
    }
}

/*
 *
 * */

PasswordLineEdit::PasswordLineEdit(QWidget *parent)
    : QLineEdit(parent)
{
    setPlaceholderText(tr("Please enter the password."));
    setEchoMode(QLineEdit::Password);
    QToolButton *button = new QToolButton(this);
    button->setObjectName("ShowPasswordButton");
    button->setCursor(Qt::PointingHandCursor);
    button->setCheckable(true);
    connect(button, &QToolButton::clicked, this, &PasswordLineEdit::onShowPassword);
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setSpacing(0);
    layout->addStretch();
    layout->addWidget(button);
}

void PasswordLineEdit::onShowPassword(bool state)
{
    setEchoMode(state? QLineEdit::Normal : QLineEdit::Password);
}

/*
 *
 * */

class AvatarWidgetPrivate{
public:
    AvatarWidgetPrivate(QWidget *parent)
        : owner(parent){
        avatarLabel = new QLabel(owner);
        avatarLabel->setText(QObject::tr("Avatar"));
        avatarLabel->setObjectName("AvatarLabel");
    }
    QWidget *owner;
    QLabel *avatarLabel;
};

AvatarWidget::AvatarWidget(QWidget *parent)
    : QWidget(parent)
    , d(new AvatarWidgetPrivate(this))
{
    QGridLayout *avatarLayout = new QGridLayout(this);
    avatarLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), 0, 2);
    avatarLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), 2, 2);
    avatarLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum), 1, 1);
    avatarLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum), 1, 3);
    avatarLayout->addWidget(d->avatarLabel, 1, 2);
}

AvatarWidget::~AvatarWidget()
{
}

void AvatarWidget::setIcon(const QPixmap &icon)
{
    d->avatarLabel->setPixmap(icon);
}
