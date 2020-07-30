#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include "dialog.h"

class MessBoxPrivate;
class MessBox : public Dialog
{
    Q_OBJECT
public:
    enum MessButton {
        YESButton,
        NOButton,
        CloseButton,
        YesAndNoButton
    };

    MessBox(QWidget *parent = nullptr);
    ~MessBox();

    static Dialog::ExecFlags Info(QWidget *parent,
                                  const QString& msg,
                                  const MessButton button = CloseButton);
    static Dialog::ExecFlags Warning(QWidget *parent,
                                     const QString& msg,
                                     const MessButton);

    void setMessage(const QString&);
    void setIconLabelObjectName(const QString&);

    void setYesAndNoButtonVisible(bool);
    void setCloseButtonVisible(bool);

private:
    void setupUI();
    void buildConnect();

    QScopedPointer<MessBoxPrivate> d;
};

#endif // MESSAGEBOX_H
