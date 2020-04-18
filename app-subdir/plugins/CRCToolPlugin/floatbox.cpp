#include "floatbox.h"

#include <QtWidgets>

union Float2Hex{
    int intData;
    float floatData;
};

class FloatBoxPrivate{
public:
    FloatBoxPrivate(QWidget *parent) : owner(parent){
        floatEdit = new QLineEdit(owner);
        floatHexEdit = new QLineEdit(owner);
    }
    QWidget *owner;
    QLineEdit *floatEdit;
    QLineEdit *floatHexEdit;
};

FloatBox::FloatBox(QWidget *parent) : QGroupBox(parent)
  , d(new FloatBoxPrivate(this))
{
    setupUI();
}

FloatBox::~FloatBox()
{
    delete d;
}

void FloatBox::onFloat2Hex()
{
    QString str = d->floatEdit->text().trimmed();
    if(str.isEmpty()) return;

    Float2Hex data;
    data.floatData = str.toFloat();

    d->floatHexEdit->setText(QString::number(data.intData, 16).toUpper());
}

void FloatBox::onHex2Float()
{
    QString str = d->floatHexEdit->text().trimmed();
    if(str.isEmpty()) return;

    Float2Hex data;
    bool ok;
    data.intData = str.toInt(&ok, 16);
    if(ok) {
        d->floatEdit->setText(QString::number(double(data.floatData)));
    }
}

void FloatBox::setupUI()
{
    setTitle(tr("Float Convert"));
    QPushButton *float2HexButton = new QPushButton(tr("To Hex"), this);
    QPushButton *hex2FloatButton = new QPushButton(tr("To Float"), this);
    connect(float2HexButton, &QPushButton::clicked, this, &FloatBox::onFloat2Hex);
    connect(hex2FloatButton, &QPushButton::clicked, this, &FloatBox::onHex2Float);

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(new QLabel(tr("Float: "), this), 0, 0);
    layout->addWidget(d->floatEdit, 0, 1);
    layout->addWidget(float2HexButton, 0, 2);
    layout->addWidget(new QLabel(tr("Float Hex: "), this), 1, 0);
    layout->addWidget(d->floatHexEdit, 1, 1);
    layout->addWidget(hex2FloatButton, 1, 2);
}
