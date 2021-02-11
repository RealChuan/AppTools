#include "maskdialog.h"

#include <graphics/imageview.h>
#include <graphics/graphicspixmapitem.h>

#include <QtWidgets>

using namespace Graphics;

class MaskDialogPrivate{
public:
    MaskDialogPrivate(QWidget *parent)
        : owner(parent){
        imageView = new ImageView(owner);
        buttonGroup = new QButtonGroup(owner);
        buttonGroup->setExclusive(true);
        penSizeSpinBox = new QSpinBox(owner);
        opacitySpinBox = new QDoubleSpinBox(owner);
        opacitySpinBox->setRange(0, 1);
        opacitySpinBox->setSingleStep(0.1);
        opacitySpinBox->setDecimals(1);
    }
    QWidget *owner;

    ImageView *imageView;
    QButtonGroup* buttonGroup;
    QSpinBox *penSizeSpinBox;
    QDoubleSpinBox *opacitySpinBox;

    QString name;
};

MaskDialog::MaskDialog(QWidget *parent)
    : Dialog(parent)
    , d_ptr(new MaskDialogPrivate(this))
{
    setWindowTitle(tr("Mask Edit"));
    setTitle(tr("Mask Edit"));
    setupUI();
    buildConnect();
    resize(parent->size());
    show();
}

MaskDialog::~MaskDialog()
{

}

void MaskDialog::setImageName(const QString &name)
{
    d_ptr->name = name;
}

void MaskDialog::setPixmap(const QPixmap &pixmap)
{
    d_ptr->imageView->setPixmap(pixmap);

    d_ptr->opacitySpinBox->setValue(0.5);
    int min = qMin(pixmap.width(), pixmap.height());
    d_ptr->penSizeSpinBox->setRange(1, min);
    d_ptr->penSizeSpinBox->setValue(min / 20.0);
}

QImage MaskDialog::maskImage()
{
    GraphicsPixmapItem* pixmapItem = d_ptr->imageView->pixmapItem();
    if(!pixmapItem)
        return QImage();
    return pixmapItem->maskImage();
}

void MaskDialog::setMaskImage(const QImage &image)
{
    GraphicsPixmapItem* pixmapItem = d_ptr->imageView->pixmapItem();
    if(!pixmapItem)
        return;
    pixmapItem->setMaskImage(image);
}

void MaskDialog::onButtonClicked(int id)
{
    GraphicsPixmapItem* pixmapItem = d_ptr->imageView->pixmapItem();
    if(!pixmapItem)
        return;
    GraphicsPixmapItem::Mode mode = GraphicsPixmapItem::Normal;
    switch (id) {
    case 1: mode= GraphicsPixmapItem::MaskDraw; break;
    case 2: mode = GraphicsPixmapItem::MaskErase; break;
    case 3: pixmapItem->clearMask(); break;
    default: break;
    }
    pixmapItem->setPaintMode(mode);
}

void MaskDialog::onPenSizeChanged(int value)
{
    GraphicsPixmapItem* pixmapItem = d_ptr->imageView->pixmapItem();
    if(!pixmapItem)
        return;
    pixmapItem->setPenSize(value);
}

void MaskDialog::onOpacityChanged(double value)
{
    GraphicsPixmapItem* pixmapItem = d_ptr->imageView->pixmapItem();
    if(!pixmapItem)
        return;
    pixmapItem->setOpacity(value);
}

void MaskDialog::onSave()
{
    QString path = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).value(0, QDir::homePath());
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd HH-mm-ss");
    time = "~" + time;
    QString name = d_ptr->name;
    int index = name.lastIndexOf('.');
    QString format = "PNG";
    if(index <= 0){
        name = name + time + ".png";
    }else{
        format = name.mid(index + 1);
        name.insert(index, time);
    }
    path = path + "/" + name;
    QString filename = QFileDialog::getSaveFileName(this, tr("Save Image"), path, tr("Images (*.png *.xpm *.jpg)"));
    if(filename.isEmpty())
        return;

    GraphicsPixmapItem* pixmapItem = d_ptr->imageView->pixmapItem();
    if(!pixmapItem)
        return;

    QPixmap pximap = pixmapItem->pixmap();
    QImage mask = pixmapItem->maskImage();
    QPainter painter(&pximap);
    painter.drawImage(pximap.rect(), mask);
    qDebug() << pximap.save(filename, format.toStdString().c_str(), 100);
}

QPushButton *newFlatBlueButton(const QString &text, QWidget *parent)
{
    QPushButton *button = new QPushButton(text, parent);
    button->setFlat(true);
    button->setObjectName("BlueButton");
    return button;
}

void MaskDialog::setupUI()
{
    QPushButton *button1 = newFlatBlueButton(tr("Mask"), this);
    QPushButton *button2 = newFlatBlueButton(tr("Erase"), this);
    QPushButton *button3 = new QPushButton(tr("Clear"), this);
    button3->setFlat(true);
    d_ptr->buttonGroup->addButton(button1, 1);
    d_ptr->buttonGroup->addButton(button2, 2);
    d_ptr->buttonGroup->addButton(button3, 3);
    d_ptr->buttonGroup->button(1)->setChecked(true);

    QPushButton *saveButton = newFlatBlueButton(tr("Save"), this);
    connect(saveButton, &QPushButton::clicked, this, &MaskDialog::onSave);

    QHBoxLayout *editLayout = new QHBoxLayout;
    editLayout->addStretch();
    editLayout->addWidget(new QLabel(tr("Pen Size: "), this));
    editLayout->addWidget(d_ptr->penSizeSpinBox);
    editLayout->addWidget(new QLabel(tr("Mask Opacity: "), this));
    editLayout->addWidget(d_ptr->opacitySpinBox);
    editLayout->addWidget(button1);
    editLayout->addWidget(button2);
    editLayout->addWidget(button3);
    editLayout->addWidget(saveButton);
    editLayout->addStretch();

    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 10);
    layout->addWidget(d_ptr->imageView);
    layout->addLayout(editLayout);

    setCentralWidget(widget);
}

void MaskDialog::buildConnect()
{
    connect(d_ptr->buttonGroup, &QButtonGroup::idClicked, this, &MaskDialog::onButtonClicked);
    connect(d_ptr->penSizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &MaskDialog::onPenSizeChanged);
    connect(d_ptr->opacitySpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MaskDialog::onOpacityChanged);
}
