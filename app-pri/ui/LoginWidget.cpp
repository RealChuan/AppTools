#include "LoginWidget.h"
#include "ui_LoginWidget.h"
#include "../help/UiSet.h"
#include "mainwindow.h"
#include "../help/Sql.h"

#pragma execution_character_set("utf-8")

class LoginWidgetPrivate{
public:
    LoginWidgetPrivate(QWidget *owner):
        owner(owner),
        lastPoint(),
        movePoint()
    {}
    QWidget *owner;
    //记录鼠标位置
    QPoint lastPoint;
    QPoint movePoint;
};

LoginWidget::LoginWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginWidget),
    d(new LoginWidgetPrivate(this))
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);   //去掉边框
    //setAttribute(Qt::WA_TranslucentBackground);//背景透明
    setAttribute(Qt::WA_StyledBackground);

    initWindow();
}

LoginWidget::~LoginWidget()
{
    delete ui;
    delete d;
}

void LoginWidget::initWindow()
{
    this->resize(260,380);
    UiSet::windowCenter(this);

    ui->picture->setText("");
    ui->tipLable->setText("");
    ui->tipLable->setAlignment(Qt::AlignCenter);

    ui->username->setPlaceholderText("用户名：admin");
    ui->password->setPlaceholderText("密码：admin");
    ui->username->setText("admin");
    ui->password->setText("admin");
    ui->password->setEchoMode(QLineEdit::Password);

    UiSet::setWidgetPaddingAndSpacing(this,5,10);
    UiSet::setWidgetPaddingAndSpacing(ui->titlewidget,0,10);

    connect(ui->closebtn,&QPushButton::clicked,[]
    {
        QApplication::quit();
    });
    connect(ui->hidebtn,&QPushButton::clicked,[this]
    {
        this->showMinimized();
    });

    ui->loginbtn->setShortcut(Qt::Key_Enter);   //登录快捷键回车

    // 输入密码后按下回车进行登陆
    connect(ui->password, &QLineEdit::returnPressed, [this] {
        ui->loginbtn->click();
    });
}

void LoginWidget::on_loginbtn_clicked()
{
    QString username=ui->username->text().trimmed();
    QString password=ui->password->text().trimmed();
    if(username.isEmpty()||password.isEmpty())
    {
        ui->tipLable->setText("用户名或密码为空");
        //QMessageBox::warning(this, "警告", "用户名不存在或者密码为空！",QMessageBox::Ok);
        return;
    }

    //增加数据库检索'账户''密码'明文
    dbSql db;
    QString sql=QString("select password from account where username='%1'").arg(username);
    qDebug()<<sql;
    QString dbpassword=db.getString(sql,"password");
    if(dbpassword.isEmpty())
    {
        ui->tipLable->setText("用户名不存在");
        //QMessageBox::warning(this, "警告", "用户名不存在！",QMessageBox::Ok);
        return;
    }
    if(dbpassword==password)
    {
        MainWindow *mainw=new MainWindow();
        this->hide();
        mainw->show();
        //this->close();//这样会出现新的窗口无法析构
        //只有在 main函数中定义并show的窗口才是主窗口
        //fix me?
        mainw->setAttribute(Qt::WA_DeleteOnClose,true);
    }
    else
        ui->tipLable->setText("密码错误");
}

//鼠标点击事件
void LoginWidget::mousePressEvent(QMouseEvent *event)
{
    //读取坐鼠标点击坐标点
    d->lastPoint = event->globalPos();
}

//鼠标移动事件
void LoginWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (this->isMaximized()) { return; }
    if(!d->lastPoint.isNull())
    {
        //把移动的点记录下来
        //int dx = event->globalX() - lastpoint.x();//这种也可以
        //int dy = event->globalY() - lastpoint.y();
        //move(x() + dx, y() + dy); //窗口移动到此处
        d->movePoint=event->globalPos()-d->lastPoint;
        d->lastPoint = event->globalPos(); //更新记录点
        move(pos()+d->movePoint);
    }
}
//鼠标释放事件
void LoginWidget::mouseReleaseEvent(QMouseEvent *)
{
    d->lastPoint=QPoint();
}
