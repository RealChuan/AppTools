#include "labelframe.h"

#include <QtWidgets>

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
    QLabel *label = new QLabel(tr("Test Page"), this);
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);
}
