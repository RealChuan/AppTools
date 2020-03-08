#include "mainwindow.h"
#include "plugindialog.h"
#include <utils/utils.h>
#include <core/mpages.h>
#include <extensionsystem/pluginmanager.h>

#include <QtWidgets>

using namespace ExtensionSystem;

class MainWindowPrivate{
public:
    MainWindowPrivate(MainWindow *parent) : owner(parent){
        switchButtonGroup = new QButtonGroup(owner);
        switchButtonGroup->setExclusive(true);
        menuButtonGroup = new QButtonGroup(owner);
        menuButtonGroup->setExclusive(true);
        pageWidget = new QStackedWidget(owner);
        v1 = new QVBoxLayout;
        v2 = new QVBoxLayout;
        v3 = new QVBoxLayout;

        v1->setContentsMargins(0, 0, 0, 0);
        v1->setSpacing(0);
        v2->setContentsMargins(0, 0, 0, 0);
        v2->setSpacing(0);
        v3->setContentsMargins(0, 0, 0, 0);
        v3->setSpacing(0);
    }

    MainWindow *owner;
    QButtonGroup *switchButtonGroup;
    QButtonGroup *menuButtonGroup;
    QStackedWidget *pageWidget;
    QVBoxLayout *v1;
    QVBoxLayout *v2;
    QVBoxLayout *v3;

    //记录鼠标位置
    QPoint lastPoint;
    QPoint movePoint;
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , d(new MainWindowPrivate(this))
{
    setWindowFlags(Qt::FramelessWindowHint);        //去掉边框
    setAttribute(Qt::WA_TranslucentBackground);     //背景透明
    setAttribute(Qt::WA_StyledBackground);
    setWindowTitle("App");
    setupUI();
}

MainWindow::~MainWindow()
{
    delete d;
    d = nullptr;
}

void MainWindow::extensionsInitialized()
{
    QVector<MPages*> mPages = PluginManager::getObjects<MPages>();
    for(const MPages* p: mPages){
        if(!p->widget()) continue;
        if(p->button()->property("Group") == MPages::Tool)
            d->v1->addWidget(p->button());
        else if(p->button()->property("Group") == MPages::About)
            d->v2->addWidget(p->button());
        else if(p->button()->property("Group") == MPages::Test)
            d->v3->addWidget(p->button());
        else
            continue;
        d->menuButtonGroup->addButton(p->button());
        d->pageWidget->addWidget(p->widget());
        connect(p->button(), &QPushButton::clicked, [=]{
            d->pageWidget->setCurrentWidget(p->widget());
        });
    }

    initMenu();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    //读取坐鼠标点击坐标点
    d->lastPoint = event->globalPos();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (isMaximized()) return;
    if(!d->lastPoint.isNull()){
        //把移动的点记录下来
        d->movePoint = event->globalPos() - d->lastPoint;
        d->lastPoint = event->globalPos();      //更新记录点
        move(pos() + d->movePoint);
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *)
{
    d->lastPoint = QPoint();
}

void MainWindow::showGroupButton(QAbstractButton *button)
{
    QVariant group = button->property("Group");
    foreach(QAbstractButton *btn, d->menuButtonGroup->buttons()){
        if(btn->property("Group") == group) btn->show();
        else btn->hide();
    }
}

void MainWindow::aboutPlugins()
{
    Core::Internal::PluginDialog dialog(this);
    dialog.exec();
}

void MainWindow::setupUI()
{
    QFrame *frame = new QFrame(this);
    QGridLayout *layout = new QGridLayout(frame);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(titleWidget(), 0, 0, 1, 2);
    layout->addWidget(menuWidget(), 1, 0, 1, 1);
    layout->addWidget(initPageWidget(tr("Hello World!")), 1, 1, 1, 1);
    layout->addWidget(new QSizeGrip(this), 1, 1, Qt::AlignRight | Qt::AlignBottom);

    setCentralWidget(frame);
    setMinimumSize(1000, 618);
    Utils::windowCenter(this);
}

QWidget *MainWindow::titleWidget()
{
    QLabel *iconLabel = new QLabel(this);
    iconLabel->setObjectName("iconLabel");
    QLabel *titleLabel = new QLabel(tr("App"), this);
    titleLabel->setObjectName("titleLabel");
    QPushButton *qssButton = new QPushButton(tr("Reload QSS"), this);
    qssButton->setObjectName("qssButton");

    QToolButton *minButton = new QToolButton(this);
    minButton->setObjectName("minButton");
    minButton->setToolTip(tr("Minimize"));
    QToolButton *maxButton = new QToolButton(this);
    maxButton->setObjectName("maxButton");
    maxButton->setToolTip(tr("Maximize"));
    QToolButton *restoreButton = new QToolButton(this);
    restoreButton->setObjectName("restoreButton");
    restoreButton->setToolTip(tr("Restore"));
    QToolButton *closeButton = new QToolButton(this);
    closeButton->setObjectName("closeButton");
    closeButton->setToolTip(tr("Close"));

    connect(qssButton, &QPushButton::clicked, this, &Utils::setQSS);
    connect(minButton, &QToolButton::clicked, this, &MainWindow::showMinimized);
    connect(maxButton, &QToolButton::clicked, [=]{
        showMaximized();
        maxButton->hide();
        restoreButton->show();
    });
    connect(restoreButton, &QToolButton::clicked, [=]{
        showNormal();
        maxButton->show();
        restoreButton->hide();
    });
    connect(closeButton, &QToolButton::clicked, this, &QApplication::quit);

    if(isMaximized()){
        maxButton->hide();
        restoreButton->show();
    }else{
        maxButton->show();
        restoreButton->hide();
    }

    QWidget *widget = new QWidget(this);
    widget->setObjectName("titleWidget");
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(5, 5, 5, 5);
    layout->setSpacing(10);
    layout->addWidget(iconLabel);
    layout->addWidget(titleLabel);
    layout->addStretch(0);
    layout->addWidget(qssButton);
    layout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Maximum, QSizePolicy::Maximum));
    layout->addWidget(minButton);
    layout->addWidget(maxButton);
    layout->addWidget(restoreButton);
    layout->addWidget(closeButton);

    return widget;
}

QWidget *MainWindow::menuWidget()
{
    QPushButton *toolsButton = new QPushButton(tr("Common Tools"), this);
    QPushButton *aboutButton = new QPushButton(tr("About"), this);
    QPushButton *pluginButton = new QPushButton(tr("About Plugins"), this);
    QPushButton *qtButton = new QPushButton(tr("About Qt"), this);
    QPushButton *testButton = new QPushButton(tr("Test"), this);

    connect(pluginButton, &QPushButton::clicked, this, &MainWindow::aboutPlugins);

    connect(qtButton, &QPushButton::clicked, [this]{
        QMessageBox::aboutQt(this);
    });

    //    toolsButton->setFlat(true);
    //    aboutButton->setFlat(true);
    //    pluginButton->setFlat(true);
    //    qtButton->setFlat(true);
    //    testButton->setFlat(true);

    toolsButton->setProperty("Group", MPages::Tool);
    aboutButton->setProperty("Group", MPages::About);
    pluginButton->setProperty("Group", MPages::About);
    qtButton->setProperty("Group", MPages::About);
    testButton->setProperty("Group", MPages::Test);

    d->switchButtonGroup->addButton(toolsButton);
    d->switchButtonGroup->addButton(aboutButton);
    d->switchButtonGroup->addButton(testButton);

    d->menuButtonGroup->addButton(pluginButton);
    d->menuButtonGroup->addButton(qtButton);

    d->v1->addWidget(toolsButton);

    d->v2->addWidget(aboutButton);
    d->v2->addWidget(pluginButton);
    d->v2->addWidget(qtButton);

    d->v3->addWidget(testButton);

    QWidget *widget = new QWidget(this);
    widget->setObjectName("menuWidget");
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addLayout(d->v1);
    layout->addLayout(d->v2);
    layout->addLayout(d->v3);
    layout->addStretch(1);

    return widget;
}

QStackedWidget *MainWindow::initPageWidget(QString str)
{
    QLabel *label = new QLabel(str, this);
    label->setObjectName("homePageLabel");
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(label);
    d->pageWidget->addWidget(widget);
    d->pageWidget->setCurrentWidget(widget);
    return d->pageWidget;
}

void MainWindow::initMenu()
{
    foreach(QAbstractButton* btn, d->switchButtonGroup->buttons()){
        btn->setLayoutDirection(Qt::LeftToRight);
        btn->setProperty("class", "GroupButton");
    }
    foreach(QAbstractButton* btn, d->menuButtonGroup->buttons()){
        btn->setProperty("class", "GroupItemButton");
        btn->setCheckable(true);
    }
    showGroupButton(d->switchButtonGroup->buttons().at(0));
    connect(d->switchButtonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(showGroupButton(QAbstractButton*)));
}

