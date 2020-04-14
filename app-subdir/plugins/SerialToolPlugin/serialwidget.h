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

private slots:
    void onSearchPort();

    void onPortChanged(const QString&);
    void onBaudRateChanged(const QString&);
    void onDataBitsChanged(const QString&);
    void onStopBitsChanged(const QString&);
    void onParityChanged(const QString&);
    void onFlowControlChanged(const QString&);

    void onOpenOrCloseSerial(bool);

private:
    void setupUI();
    void init();
    void buildConnect();

    SerialWidgetPrivate *d;
};

#endif // SERIALWIDGET_H
