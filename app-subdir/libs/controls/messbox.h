#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include "controls_global.h"
#include "commonwidget.h"

class MessBoxPrivate;
class CONTROLS_EXPORT MessBox : public CommonWidget
{
    Q_OBJECT
public:
    MessBox(QWidget *parent = nullptr);
    ~MessBox();

    static void Info(QWidget *parent, const QString& msg);
    static void Warning(QWidget *parent, const QString& msg);

    void exec();
    void setMessage(const QString&);
    void setIconLabelObjectName(const QString&);

private:
    void setupUI();
    void buildConnect();

    MessBoxPrivate *d;

};

#endif // MESSAGEBOX_H
