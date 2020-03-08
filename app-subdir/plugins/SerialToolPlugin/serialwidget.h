#ifndef SERIALWIDGET_H
#define SERIALWIDGET_H

#include <QWidget>

class SerialWidgetPrivate;
class SerialWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SerialWidget(QWidget *parent = nullptr);
    ~SerialWidget();

signals:

private:
    void setupUI();

    SerialWidgetPrivate *d;
};

#endif // SERIALWIDGET_H
