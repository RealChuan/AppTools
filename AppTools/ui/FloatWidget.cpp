#include "FloatWidget.h"
#include "ui_FloatWidget.h"
#include "../help/BinaryCvn.h"
#include <QDebug>

FloatWidget::FloatWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FloatWidget)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);   //去掉边框
    setAttribute(Qt::WA_StyledBackground);
    QApplication::setFont(QFont ("微软雅黑", 10, QFont::Normal, false));
}

FloatWidget::~FloatWidget()
{
    delete ui;
}

void FloatWidget::on_convHex_clicked()
{
    if(ui->FloatLine->text().isEmpty())
    {
        return;
    }
    data.floater = ui->FloatLine->text().toFloat();

    //qDebug()<<Hex;
    ui->HexLine->setText(QString("%1").arg(data.inter,8,16,QChar('0')));
}

void FloatWidget::on_convFlaot_clicked()
{
    if(ui->HexLine->text().isEmpty())
    {
        return;
    }
    bool ok;
    QString h=ui->HexLine->text();
    data.inter=h.toInt(&ok,16);
//    qDebug()<<ok;
//    qDebug()<<hex<<data.inter;
    ui->FloatLine->setText(QString("%1").arg(static_cast<double>(data.floater)));
}
