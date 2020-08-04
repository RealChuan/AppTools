#ifndef CRCWIDGET_H
#define CRCWIDGET_H

#include <QWidget>

class QGroupBox;
class CRCWidgetPrivate;
class CRCWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CRCWidget(QWidget *parent = nullptr);
    ~CRCWidget();

private slots:
    void onModelChanged(int);
    void onCalculate();

private:
    void setupUI();
    void init();
    void buildConnect();

    QScopedPointer<CRCWidgetPrivate> d;
};

#endif // CRCWIDGET_H
