#include "crcwidget.h"
#include "floatbox.h"
#include "systemconversionbox.h"

#include <utils/crcalgorithm.h>

#include <QtWidgets>

using namespace CRC;

class CRCWidgetPrivate{
public:
    CRCWidgetPrivate(QWidget *parent)
        : owner(parent){
        resultEdit = new QLineEdit(owner);
        resultEdit->setReadOnly(true);
        modelBox = new QComboBox(owner);
        inputEdit = new QTextEdit(owner);

        nameLabel = new QLabel(QObject::tr("-"), owner);
        widthLabel = new QLabel(QObject::tr("-"), owner);
        polyFormulaLabel = new QLabel(QObject::tr("-"), owner);
        polyFormulaLabel->setWordWrap(true);
        polyLabel = new QLabel(QObject::tr("-"), owner);
        initLabel = new QLabel(QObject::tr("-"), owner);
        reverseInputLable = new QLabel(QObject::tr("-"), owner);
        reverseOutputLable = new QLabel(QObject::tr("-"), owner);
        xOrOutLabel = new QLabel(QObject::tr("-"), owner);
    }
    QWidget *owner;

    QLineEdit *resultEdit;
    QComboBox *modelBox;
    QTextEdit *inputEdit;

    QLabel *nameLabel;
    QLabel *widthLabel;
    QLabel *polyFormulaLabel;
    QLabel *polyLabel;
    QLabel *initLabel;
    QLabel *reverseInputLable;
    QLabel *reverseOutputLable;
    QLabel *xOrOutLabel;
};

CRCWidget::CRCWidget(QWidget *parent)
    : QWidget(parent)
    , d(new CRCWidgetPrivate(this))
{
    setupUI();
    init();
    buildConnect();
    d->modelBox->setCurrentIndex(7);
}

CRCWidget::~CRCWidget()
{
}

void CRCWidget::onModelChanged(int index)
{
    CRCModel model = CRCModel(index);
    d->nameLabel->setText(d->modelBox->currentText());
    d->widthLabel->setText(QString::number(getBitsWidth(model)));
    d->polyFormulaLabel->setText(QString::fromStdString(getPolyFormula(model)));
    d->polyLabel->setText(QString::number(getPoly(model), 16));
    d->initLabel->setText(QString::number(getInitValue(model), 16));
    d->reverseInputLable->setText(getInputReversal(model)? tr("true"): tr("false"));
    d->reverseOutputLable->setText(getOutputReversal(model)? tr("true"): tr("false"));
    d->xOrOutLabel->setText(QString::number(getXorValue(model), 16));
}

void CRCWidget::onCalculate()
{
    QString buf = d->inputEdit->toPlainText();
    QByteArray bytes = QByteArray::fromHex(buf.toLocal8Bit());


    CRCModel model = CRCModel(d->modelBox->currentIndex());
    uint8_t* data = reinterpret_cast<uint8_t*>(bytes.data());
    quint64 length = quint64(bytes.length());

    int bitsWidth = getBitsWidth(model);
    uint8_t crc8;
    uint16_t crc16;
    uint32_t crc32;
    QString str;
    switch (bitsWidth) {
    case 8:
        crc8 = crcCalculate<uint8_t>(data, length, model);
        str = QString::number(crc8, 16).toUpper();
        //crc.append(reinterpret_cast<char*>(&crc8), 1);
        break;
    case 16:
        crc16 = crcCalculate<uint16_t>(data, length, model);
        str = QString("%1 %2").arg(QString::number(crc16 >> 8, 16)).toUpper().
                arg(QString::number(crc16 & 0xff, 16)).toUpper();
        //crc.append(reinterpret_cast<char*>(&crc16), 2);
        break;
    case 32:
        crc32 = crcCalculate<uint32_t>(data, length, model);
        str = QString("%1 %2 %3 %4").arg(QString::number(crc32 >> 24, 16)).toUpper().
                arg(QString::number((crc32 >> 16) & 0xff, 16)).toUpper().
                arg(QString::number((crc32 >> 8) & 0xff, 16)).toUpper().
                arg(QString::number(crc32 & 0xff, 16)).toUpper();
        //crc.append(reinterpret_cast<char*>(&crc32), 4);
        break;
    default:
        break;
    }

    //QString str = formatHex(crc);
    d->resultEdit->setText(str);
}

void CRCWidget::setupUI()
{
    setObjectName("CRCWidget");
    QHBoxLayout *outLayout = new QHBoxLayout;
    outLayout->addWidget(new QLabel(tr("CRC (CRCH CRCL): "), this));
    outLayout->addWidget(d->resultEdit);

    QPushButton *calButton = new QPushButton(tr("Calculate"), this);
    calButton->setObjectName("CalculateButton");
    connect(calButton, &QPushButton::clicked, this, &CRCWidget::onCalculate);

    QGroupBox *infoBox = new QGroupBox(tr("Information"), this);
    infoBox->setObjectName("CRCInfoBox");
    QGridLayout *infoLayout = new QGridLayout(infoBox);
    infoLayout->addWidget(new QLabel(tr("Name: "), this), 0, 0, 1, 1);
    infoLayout->addWidget(d->nameLabel, 0, 1, 1, 1);
    infoLayout->addWidget(new QLabel(tr("Width: "), this), 1, 0, 1, 1);
    infoLayout->addWidget(d->widthLabel, 1, 1, 1, 1);
    infoLayout->addWidget(new QLabel(tr("Poly Formula: "), this), 2, 0, 1, 1);
    infoLayout->addWidget(d->polyFormulaLabel, 2, 1, 1, 1);
    infoLayout->addWidget(new QLabel(tr("Poly: "), this), 3, 0, 1, 1);
    infoLayout->addWidget(d->polyLabel, 3, 1, 1, 1);
    infoLayout->addWidget(new QLabel(tr("Init: "), this), 4, 0, 1, 1);
    infoLayout->addWidget(d->initLabel, 4, 1, 1, 1);
    infoLayout->addWidget(new QLabel(tr("Reverse Input: "), this), 5, 0, 1, 1);
    infoLayout->addWidget(d->reverseInputLable, 5, 1, 1, 1);
    infoLayout->addWidget(new QLabel(tr("Reverse Output: "), this), 6, 0, 1, 1);
    infoLayout->addWidget(d->reverseOutputLable, 6, 1, 1, 1);
    infoLayout->addWidget(new QLabel(tr("XOrOut: "), this), 7, 0, 1, 1);
    infoLayout->addWidget(d->xOrOutLabel, 7, 1, 1, 1);

    //    QFormLayout *formLayout = new QFormLayout(infoBox);
    //    formLayout->addRow(tr("Name: "), d->nameLabel);
    //    formLayout->addRow(tr("Width: "), d->widthLabel);
    //    formLayout->addRow(tr("Poly Formula: "), d->polyFormulaLabel);
    //    formLayout->addRow(tr("Poly: "), d->polyLabel);
    //    formLayout->addRow(tr("Init: "), d->initLabel);
    //    formLayout->addRow(tr("Reverse Input: "), d->reverseInputLable);
    //    formLayout->addRow(tr("Reverse Output: "), d->reverseOutputLable);
    //    formLayout->addRow(tr("XOrOut: "), d->xOrOutLabel);

    QGroupBox *CRCBox = new QGroupBox(tr("CRC Calculator"), this);
    QGridLayout *crcLayout = new QGridLayout(CRCBox);
    crcLayout->addLayout(outLayout, 0, 0);
    crcLayout->addWidget(d->modelBox, 0, 1);
    crcLayout->addWidget(d->inputEdit, 1, 0, 2, 1);
    crcLayout->addWidget(infoBox, 1, 1);
    crcLayout->addWidget(calButton, 2, 1);

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(CRCBox, 0, 0, 1, 2);
    layout->addWidget(new FloatBox(this), 1, 0, 1, 1);
    layout->addWidget(new SystemConversionBox(this), 1, 1, 1, 1);
}

void CRCWidget::init()
{
    for(int i=0; i<CRC_MODEL_SIZE; i++)
        d->modelBox->addItem(CRC_MODEL[i]);
}

void CRCWidget::buildConnect()
{
    connect(d->modelBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onModelChanged(int)));
}
