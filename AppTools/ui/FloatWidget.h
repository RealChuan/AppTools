#ifndef FLOATWIDGET_H
#define FLOATWIDGET_H

#include <QWidget>

namespace Ui {
class FloatWidget;
}

class FloatWidget : public QWidget
{
    Q_OBJECT

public:
    //联合体
    union binaryCvn
    {
        int inter;
        float floater;
    };
    explicit FloatWidget(QWidget *parent = nullptr);
    ~FloatWidget();

private slots:
    void on_convHex_clicked();

    void on_convFlaot_clicked();

private:
    Ui::FloatWidget *ui;
    binaryCvn data;
};

#endif // FLOATWIDGET_H
