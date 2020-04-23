#ifndef REGISTERWIDGET_H
#define REGISTERWIDGET_H

#include <controls/dialog.h>

class RegisterWidgetPrivate;
class RegisterWidget : public Dialog
{
    Q_OBJECT
public:
    explicit RegisterWidget(QWidget *parent = nullptr);
    ~RegisterWidget();

signals:

private slots:
    void onRegister();

private:
    void setupUI();

    RegisterWidgetPrivate *d;
};

#endif // REGISTERWIDGET_H
