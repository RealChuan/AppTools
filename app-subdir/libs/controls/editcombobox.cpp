#include "editcombobox.h"

#include <QtWidgets>

class AccountItemPrivate{
public:
    AccountItemPrivate(QWidget *parent) : owner(parent){
        avatarLabel = new QLabel("avatar", owner);
        usernameLabel = new QLabel("username", owner);
    }
    QWidget *owner;
    QLabel *avatarLabel;
    QLabel *usernameLabel;
    bool mousePress = false;
};

AccountItemWidget::AccountItemWidget(QWidget *parent) : QWidget(parent)
  , d(new AccountItemPrivate(this))
{
    setupUI();
}

AccountItemWidget::~AccountItemWidget()
{
    delete d;
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
        d->mousePress = true;
}

void AccountItemWidget::mouseReleaseEvent(QMouseEvent *)
{
    if(d->mousePress){
        emit showAccount(d->usernameLabel->text());
        d->mousePress = false;
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
    layout->addStretch(0);
    layout->addWidget(deleteButton);
    layout->setContentsMargins(5, 5, 5, 5);
    layout->setSpacing(5);
}

/*
 *
 *
 *
 * */


class EditComboBoxPrivate{
public:
    EditComboBoxPrivate(QWidget *parent) : owner(parent){
        listWidget = new QListWidget(owner);
    }
    QWidget *owner;
    QListWidget *listWidget;
};

EditComboBox::EditComboBox(QWidget *parent) : QComboBox(parent)
  , d(new EditComboBoxPrivate(this))
{
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
    QListWidgetItem *item = new QListWidgetItem(d->listWidget);
    d->listWidget->setItemWidget(item, accountItem);
    connect(accountItem, &AccountItemWidget::showAccount, this, &EditComboBox::onShowText);
    connect(accountItem, &AccountItemWidget::remove, this, &EditComboBox::onDeleteItem);
}

void EditComboBox::onShowText(const QString &text)
{
    setEditText(text);
    hidePopup();
}

void EditComboBox::onDeleteItem(const QString &text)
{
    QMessageBox::warning(this, "Warning", "delete this account!",QMessageBox::Yes);

    for(int i=0; i<d->listWidget->count(); i++){
        QListWidgetItem *item = d->listWidget->item(i);
        AccountItemWidget *accountItem = qobject_cast<AccountItemWidget*>(d->listWidget->itemWidget(item));
        if(accountItem->getAccount() == text){
            delete d->listWidget->takeItem(i);
            break;
        }
    }
}
