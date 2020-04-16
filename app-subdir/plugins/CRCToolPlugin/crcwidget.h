#ifndef CRCWIDGET_H
#define CRCWIDGET_H

#include <QWidget>

class CRCWidgetPrivate;
class CRCWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CRCWidget(QWidget *parent = nullptr);
    ~CRCWidget();

signals:

private slots:
    void onModelChanged(int);
    void onCalculate();

private:
    void setupUI();
    void init();
    void buildConnect();

    CRCWidgetPrivate*d;

};

#endif // CRCWIDGET_H
