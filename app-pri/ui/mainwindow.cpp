#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../help/UiSet.h"
#include "../help/AppCfg.h"
#include "SerialWidget.h"
#include "TcpWidget.h"
#include "CustomWidget.h"
#include "FloatWidget.h"
#include "CrcWidget.h"
#include "LoginWidget.h"

#pragma execution_character_set("utf-8")

class MainWindowPrivate{
public:
    MainWindowPrivate(QWidget *owner)
        :owner(owner)
    {
        sizeGrip=new QSizeGrip(nullptr);
        topButtonGroup = new QButtonGroup(owner);
        topButtonGroup->setExclusive(true);
        switchButtonGroup = new QButtonGroup(owner);

        listButtonGroup = new QButtonGroup(owner);
        listButtonGroup->setExclusive(true);
    }
    QWidget *owner;
    //记录鼠标位置
    QPoint lastPoint;
    QPoint movePoint;
    //右下角缩放功能
    QSizeGrip *sizeGrip;
    //界面map
    QMap<QString,QWidget*> menuMap;
    QButtonGroup *topButtonGroup;
    QButtonGroup *switchButtonGroup;
    QList<QPushButton*> itemButton;

    QButtonGroup *listButtonGroup;
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    ,d(new MainWindowPrivate(this))
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);   //去掉边框
    setAttribute(Qt::WA_TranslucentBackground);//背景透明
    setAttribute(Qt::WA_StyledBackground);

    initWindow();
    setPaddingAndSpacing();
    titleBtn();

    //initListButton();
    //ui->menubtnwidget->hide();
    //树形菜单
    initMenuButton();
    ui->listbtnwidget->hide();

    //    保存配置
    //    QObject::connect(qApp,&QCoreApplication::aboutToQuit,[]
    //    {
    //        AppCfg::writeConfig();
    //    });
}

MainWindow::~MainWindow()
{
    delete ui;
    delete d;
}

void MainWindow::initWindow()
{
    this->resize(1000,600);
    UiSet::windowCenter(this);
    setWindowTitle("AppTools");

    QGridLayout *l = qobject_cast<QGridLayout*>(ui->centralwidget->layout());
    // 添加 size grip 到窗口右下角
    l->addWidget(d->sizeGrip, 1, 4, Qt::AlignRight | Qt::AlignBottom);

    ui->label->setFont(QFont("微软雅黑", 14, QFont::Normal, false));
    ui->label->setAlignment(Qt::AlignCenter);
    ui->label->setText("AppTools");

    QWidget *myWidget=new CustomWidget(QString("你好呀!"),this);
    if(myWidget!=nullptr)
    {
        ui->stackedWidget->addWidget(myWidget);
        ui->stackedWidget->setCurrentWidget(myWidget);
        myWidget->setAttribute(Qt::WA_DeleteOnClose,true);
    }
}

void MainWindow::initMenuButton()
{
    ui->toolsbtngroup->setProperty("name","one");
    ui->tcpbtnitem->setProperty("name","one");
    ui->serialbtnitem->setProperty("name","one");
    ui->floatbtnitem->setProperty("name","one");
    ui->crcbtnitem->setProperty("name","one");
    ui->setbtngroup->setProperty("name","two");
    ui->accountbtnitem->setProperty("name","two");
    ui->aboutqtitem->setProperty("name","two");
    QList<QPushButton *> allPButtons = ui->menubtnwidget->findChildren<QPushButton *>();
    foreach(QPushButton *btn,allPButtons)
    {
        btn->setFlat(true);
        if(btn->text()=="常用工具"||btn->text()=="设置")
        {
            btn->setLayoutDirection(Qt::LeftToRight);
            d->topButtonGroup->addButton(btn);
            btn->setProperty("class","GroupButton");
        }
        else
        {
            btn->setProperty("class","GroupItemButton");
            btn->setCheckable(true);
            d->itemButton.append(btn);
            d->switchButtonGroup->addButton(btn);
        }
    }
    connect(d->topButtonGroup,
            static_cast<void (QButtonGroup::*)(QAbstractButton*)>(&QButtonGroup::buttonClicked),
            [this](QAbstractButton *button)
    {
        QString name = button->property("name").toString();
        foreach(QPushButton *btn , d->itemButton)
        {
            if(btn->property("name").toString() == name)
                btn->show();
            else
                btn->hide();
        }
    });
    connect(d->switchButtonGroup,
            static_cast<void (QButtonGroup::*)(QAbstractButton*)>(&QButtonGroup::buttonClicked),
            [this](QAbstractButton *button)
    {
        QString className=button->text();
        if(className.isEmpty())
            return;
        if(!d->menuMap.contains(className))
            createMenuMap(className);
        else
            ui->stackedWidget->setCurrentWidget(d->menuMap.value(className));
    });
    connect(ui->aboutqtitem,&QPushButton::clicked,[this]
    {
        QMessageBox::aboutQt(this);
    });
    ui->toolsbtngroup->click();
}

void MainWindow::initListButton()
{
    QList<QPushButton *> allPButtons = ui->listbtnwidget->findChildren<QPushButton *>();
    foreach(QPushButton *btn,allPButtons)
    {
        btn->setCheckable(true);
        btn->setFlat(true);
        btn->setProperty("class","ListGroupButton");
        d->listButtonGroup->addButton(btn);
    }
    connect(d->listButtonGroup,
            static_cast<void (QButtonGroup::*)(QAbstractButton*)>(&QButtonGroup::buttonClicked),
            [this](QAbstractButton *button)
    {
        QString className=button->text();
        if(className.isEmpty())
            return;
        if(!d->menuMap.contains(className))
            createMenuMap(className);
        else
            ui->stackedWidget->setCurrentWidget(d->menuMap.value(className));
    });
    connect(ui->qtbtn,&QPushButton::clicked,[this]
    {
        QMessageBox::aboutQt(this);
    });
}

void MainWindow::setPaddingAndSpacing()
{
    UiSet::setWidgetPaddingAndSpacing(this,0,0);
    UiSet::setWidgetPaddingAndSpacing(ui->centralwidget,0,0);
    UiSet::setWidgetPaddingAndSpacing(ui->titlewidget,5,10);
    UiSet::setWidgetPaddingAndSpacing(ui->menubtnwidget,0,0);
    UiSet::setWidgetPaddingAndSpacing(ui->listbtnwidget,0,0);
}

void MainWindow::titleBtn()
{
    ui->closebtn->setToolTip("关闭");
    ui->hidebtn->setToolTip("最小化");
    ui->maxbtn->setToolTip("最大化");
    ui->minbtn->setToolTip("缩小窗口");
    if(!this->isMaximized())
    {
        ui->minbtn->hide();
        ui->maxbtn->show();
    }
    else
    {
        ui->maxbtn->hide();
        ui->minbtn->show();
    }
    connect(ui->closebtn,&QPushButton::clicked,[this]
    {
        close();
    });
    connect(ui->hidebtn,&QPushButton::clicked,[this]
    {
        showMinimized();
    });
    connect(ui->maxbtn,&QPushButton::clicked,[this]
    {
        showMaximized();
        ui->maxbtn->hide();
        ui->minbtn->show();
    });
    connect(ui->minbtn,&QPushButton::clicked,[this]
    {
        showNormal();
        ui->maxbtn->show();
        ui->minbtn->hide();
    });

    connect(ui->qssbtn,&QPushButton::clicked,[]
    {
        //UiSet::setQSS(QString("%1/qss/mac.css").arg(qApp->applicationDirPath()));
        UiSet::setQSS();
        qDebug()<<"重新加载QSS";
    });
}

void MainWindow::createMenuMap(QString className)
{
    QWidget *myWidget=nullptr;
    if(className=="串口助手")
        myWidget = new SerialWidget(this);
    else if(className=="TCP助手")
        myWidget=new TcpWidget(this);
    else if(className=="浮点数助手")
        myWidget=new FloatWidget(this);
    else if(className=="CRC助手")
        myWidget=new CrcWidget(this);
    if(myWidget!=nullptr)
    {
        d->menuMap.insert(className,myWidget);
        ui->stackedWidget->addWidget(myWidget);
        ui->stackedWidget->setCurrentWidget(myWidget);
        myWidget->setAttribute(Qt::WA_DeleteOnClose,true);
    }
}

//鼠标点击事件
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    //读取坐鼠标点击坐标点
    d->lastPoint = event->globalPos();
}

//鼠标移动事件
void MainWindow::mouseMoveEvent(QMouseEvent *event)
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
void MainWindow::mouseReleaseEvent(QMouseEvent *)
{
    d->lastPoint=QPoint();
}
