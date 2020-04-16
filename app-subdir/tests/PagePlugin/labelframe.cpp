#include "labelframe.h"

#include <utils/crcalgorithm.h>

#include <QtWidgets>

using namespace CRC;

LabelFrame::LabelFrame(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void LabelFrame::setupUI()
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    QLabel *label = new QLabel(this);
    label->setWordWrap(true);

    QString str;
    for(int i=0; i<CRC_MODEL_SIZE; i++){
        str += QString::fromLocal8Bit(CRC_MODEL[i]) + "\n";
    }

    QString buf = "11 22 33 44 55";
    QByteArray data = QByteArray::fromHex(buf.toLocal8Bit());

    uint16_t crc16 = CRC::crcCalculate<uint16_t>(reinterpret_cast<uint8_t*>(data.data()), static_cast<quint64>(data.length()), CRC_16_MODBUS);

    qDebug() << hex << crc16;

    label->setText(str);
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);
}
