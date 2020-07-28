#ifndef EIDTCOMBOBOX_H
#define EIDTCOMBOBOX_H

#include "controls_global.h"

#include <QComboBox>
#include <QLineEdit>

class AccountItemPrivate;
class AccountItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AccountItemWidget(QWidget *parent = nullptr);
    ~AccountItemWidget() override ;

    void setAvatar(const QString&);
    void setUsername(const QString&);

    QString getAccount() const;

signals:
    void remove(const QString&);
    void showAccount(const QString&);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void onRemove();

private:
    void setupUI();

    QScopedPointer<AccountItemPrivate> d;
};

class EditComboBoxPrivate;
class CONTROLS_EXPORT EditComboBox : public QComboBox
{
    Q_OBJECT
public:
    EditComboBox(QWidget *parent = nullptr);
    ~EditComboBox();

    void addAccount(const QString&);
    QStringList accountList() const;

private slots:
    void onShowText(const QString&);
    void onDeleteItem(const QString&);

private:
    EditComboBoxPrivate *d;
};

class CONTROLS_EXPORT PasswordLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    PasswordLineEdit(QWidget *parent = nullptr);

private slots:
    void onShowPassword(bool);
};

class AvatarWidgetPrivate;
class CONTROLS_EXPORT AvatarWidget : public QWidget
{
public:
    AvatarWidget(QWidget *parent = nullptr);
    ~AvatarWidget();

    void setIcon(const QPixmap&);

private:
    QScopedPointer<AvatarWidgetPrivate> d;
};

#endif // EIDTCOMBOBOX_H
