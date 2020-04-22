#ifndef EIDTCOMBOBOX_H
#define EIDTCOMBOBOX_H

#include "controls_global.h"

#include <QComboBox>

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

    AccountItemPrivate *d;
};

class EditComboBoxPrivate;
class CONTROLS_EXPORT EditComboBox : public QComboBox
{
    Q_OBJECT
public:
    EditComboBox(QWidget *parent = nullptr);
    ~EditComboBox();

    void addAccount(const QString&);

private slots:
    void onShowText(const QString&);
    void onDeleteItem(const QString&);

private:
    EditComboBoxPrivate *d;
};

#endif // EIDTCOMBOBOX_H
