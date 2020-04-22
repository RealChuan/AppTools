#include "labelframe.h"

#include <utils/crcalgorithm.h>
#include <controls/editcombobox.h>

#include <QtWidgets>

using namespace CRC;

LabelFrame::LabelFrame(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void LabelFrame::setupUI()
{
    EditComboBox *box = new EditComboBox(this);
    QPushButton *button = new QPushButton("add", this);
    connect(button, &QPushButton::clicked, [=]{
        box->addAccount(box->currentText());
    });
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(box);
    layout->addWidget(button);
}
