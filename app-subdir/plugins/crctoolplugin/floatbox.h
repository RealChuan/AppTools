#ifndef FLOATBOX_H
#define FLOATBOX_H

#include <QGroupBox>

class FloatBoxPrivate;
class FloatBox : public QGroupBox
{
    Q_OBJECT
public:
    FloatBox(QWidget *parent = nullptr);

private slots:
    void onFloat2Hex();
    void onHex2Float();

private:
    void setupUI();

    QScopedPointer<FloatBoxPrivate> d;
};

#endif // FLOATBOX_H
