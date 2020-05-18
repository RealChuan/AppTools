#include "systemconversionbox.h"

#include <QtWidgets>

class SystemConversionBoxPrivate{
public:
    SystemConversionBoxPrivate(QWidget *parent) : owner(parent){
        decimalEdit = new QLineEdit(owner);
        binaryEdit = new QLineEdit(owner);
        octalEdit = new QLineEdit(owner);
        hexEdit = new QLineEdit(owner);

        binaryEdit->setReadOnly(true);
        octalEdit->setReadOnly(true);
    }
    QWidget *owner;
    QLineEdit *decimalEdit;
    QLineEdit *binaryEdit;
    QLineEdit *octalEdit;
    QLineEdit *hexEdit;
};

SystemConversionBox::SystemConversionBox(QWidget *parent) : QGroupBox(parent)
  , d(new SystemConversionBoxPrivate(this))
{
    setupUI();
    buildConnect();
}

void SystemConversionBox::onDecimalChanged(const QString &text)
{
    QString dec = text.trimmed();
    if(dec.isEmpty()) return;

    long long data = dec.toLongLong();
    d->binaryEdit->blockSignals(true);
    d->octalEdit->blockSignals(true);
    d->hexEdit->blockSignals(true);
    d->binaryEdit->setText(QString::number(data, 2).toUpper());
    d->octalEdit->setText(QString::number(data, 8).toUpper());
    d->hexEdit->setText(QString::number(data, 16).toUpper());
    d->binaryEdit->blockSignals(false);
    d->octalEdit->blockSignals(false);
    d->hexEdit->blockSignals(false);
}

void SystemConversionBox::onHexChanged(const QString &text)
{
    QString hex = text.trimmed();
    if(hex.isEmpty()) return;

    long long data = hex.toLocal8Bit().toHex().toLongLong();
    d->decimalEdit->blockSignals(true);
    d->binaryEdit->blockSignals(true);
    d->octalEdit->blockSignals(true);
    d->decimalEdit->setText(QString::number(data));
    d->binaryEdit->setText(QString::number(data, 2).toUpper());
    d->octalEdit->setText(QString::number(data, 8).toUpper());
    d->decimalEdit->blockSignals(false);
    d->binaryEdit->blockSignals(false);
    d->octalEdit->blockSignals(false);
}

void SystemConversionBox::setupUI()
{
    setTitle(tr("System Conversion"));

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(new QLabel(tr("Decimal: "), this), 0, 0, 1, 1);
    layout->addWidget(d->decimalEdit, 0, 1, 1, 1);
    layout->addWidget(new QLabel(tr("Binary: "), this), 1, 0, 1, 1);
    layout->addWidget(d->binaryEdit, 1, 1, 1, 1);
    layout->addWidget(new QLabel(tr("Octal: "), this), 2, 0, 1, 1);
    layout->addWidget(d->octalEdit, 2, 1, 1, 1);
    layout->addWidget(new QLabel(tr("Octal: "), this),3, 0, 1, 1);
    layout->addWidget(d->hexEdit, 3, 1, 1, 1);

    //    QFormLayout *layout = new QFormLayout(this);
    //    layout->addRow(tr("Decimal: "), d->decimalEdit);
    //    layout->addRow(tr("Binary: "), d->binaryEdit);
    //    layout->addRow(tr("Octal: "), d->octalEdit);
    //    layout->addRow(tr("Hex: "), d->hexEdit);
}

void SystemConversionBox::buildConnect()
{
    connect(d->decimalEdit, &QLineEdit::textChanged, this, &SystemConversionBox::onDecimalChanged);
    connect(d->hexEdit, &QLineEdit::textChanged, this, &SystemConversionBox::onHexChanged);
}
