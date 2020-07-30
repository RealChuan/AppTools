#include "mainwindow.h"
#include "configwidget.h"

#include <utils/utils.h>
#include <useraccountsystem/useraccountsystem.h>
#include <tools/tcptool/tcpwidget.h>
#include <tools/serialtool/serialwidget.h>
#include <tools/crctool/crcwidget.h>

#include <QtWidgets>

class MainWindowPrivate{
public:
    MainWindowPrivate(MainWindow *parent)
        : owner(parent){
        switchButtonGroup = new QButtonGroup(owner);
        switchButtonGroup->setExclusive(true);
        menuButtonGroup = new QButtonGroup(owner);
        menuButtonGroup->setExclusive(true);
        pageWidget = new QStackedWidget(owner);
        v1 = new QVBoxLayout;
        v2 = new QVBoxLayout;

        v1->setContentsMargins(0, 0, 0, 0);
        v1->setSpacing(0);
        v2->setContentsMargins(0, 0, 0, 0);
        v2->setSpacing(0);

        accountButton = new QToolButton(owner);
        accountButton->setCheckable(true);
        accountButton->setObjectName("AccountButton");
        accountButton->setToolTip(QObject::tr("Account control"));
        userSystem = new UserAccountSystem(owner);
    }

    MainWindow *owner;
    QMap<QString, QWidget*> menuMap;
    QButtonGroup *switchButtonGroup;
    QButtonGroup *menuButtonGroup;
    QStackedWidget *pageWidget;
    QVBoxLayout *v1;
    QVBoxLayout *v2;

    QToolButton *accountButton;
    UserAccountSystem *userSystem;
};

MainWindow::MainWindow(QWidget *parent)
    : CommonWidget(parent)
    , d(new MainWindowPrivate(this))
{
    setWindowTitle("AppPlugin");
    setupUI();
}

MainWindow::~MainWindow()
{
}

void MainWindow::onShowGroupButton(QAbstractButton *button)
{
    QVariant group = button->property("Group");
    for(QAbstractButton *btn: d->menuButtonGroup->buttons()){
        if(btn->property("Group") == group)
            btn->show();
        else
            btn->hide();
    }
}

void MainWindow::onShowWidget(QAbstractButton *button)
{
    QString name = button->text();
    if(name.isEmpty())
        return;
    if(!d->menuMap.contains(name))
        createMenuMap(name);
    else
        d->pageWidget->setCurrentWidget(d->menuMap.value(name));
}

void MainWindow::onAccount(bool state)
{
    d->accountButton->setChecked(!state);
    d->userSystem->show();
}

void MainWindow::setupUI()
{
    initToolBar();

    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(menuWidget());
    layout->addWidget(initPageWidget(tr("Hello World!")));

    setCentralWidget(widget);
    setMinimumSize(1000, 618);
    Utils::windowCenter(this);

    connect(this, &MainWindow::aboutToclose, qApp, &QApplication::quit);
}

void MainWindow::initToolBar()
{
    QPushButton *qssButton = new QPushButton(tr("Reload QSS"), this);
    qssButton->setObjectName("QssButton");
    connect(qssButton, &QPushButton::clicked, this, &Utils::setQSS);

    connect(d->accountButton, &QToolButton::clicked, this, &MainWindow::onAccount);
    connect(d->userSystem, &UserAccountSystem::login,
            d->accountButton, &QToolButton::setChecked);

    QToolButton *configButton = new QToolButton(this);
    configButton->setObjectName("ConfigButton");
    configButton->setToolTip(tr("Set up"));
    ConfigWidget *configWidget = new ConfigWidget(this);
    d->pageWidget->addWidget(configWidget);
    connect(configButton, &QToolButton::clicked, [=]{
        d->pageWidget->setCurrentWidget(configWidget);
    });

    QWidget *titleBar = new QWidget(this);
    QHBoxLayout *titleLayout = new QHBoxLayout(titleBar);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->setSpacing(10);
    titleLayout->addWidget(qssButton);
    titleLayout->addWidget(d->accountButton);
    titleLayout->addWidget(configButton);
    setTitleBar(titleBar);
}

QWidget *MainWindow::menuWidget()
{
    QPushButton *toolsButton = new QPushButton(tr("Common Tools"), this);
    QPushButton *tcpButton = new QPushButton(tr("Tcp Assistant"), this);
    QPushButton *serialButton = new QPushButton(tr("Serial Assistant"), this);
    QPushButton *crcButton = new QPushButton(tr("CRC Assistant"), this);
    QPushButton *aboutButton = new QPushButton(tr("About"), this);
    QPushButton *qtButton = new QPushButton(tr("About Qt"), this);

    toolsButton->setFlat(true);
    tcpButton->setFlat(true);
    serialButton->setFlat(true);
    crcButton->setFlat(true);
    aboutButton->setFlat(true);
    qtButton->setFlat(true);

    toolsButton->setProperty("Group", Tool);
    tcpButton->setProperty("Group", Tool);
    serialButton->setProperty("Group", Tool);
    crcButton->setProperty("Group", Tool);
    aboutButton->setProperty("Group", About);
    qtButton->setProperty("Group", About);

    connect(qtButton, &QPushButton::clicked, [this]{ QMessageBox::aboutQt(this); });

    d->switchButtonGroup->addButton(toolsButton);
    d->switchButtonGroup->addButton(aboutButton);

    d->menuButtonGroup->addButton(tcpButton);
    d->menuButtonGroup->addButton(serialButton);
    d->menuButtonGroup->addButton(crcButton);
    d->menuButtonGroup->addButton(qtButton);

    initMenu();

    d->v1->addWidget(toolsButton);
    d->v1->addWidget(tcpButton);
    d->v1->addWidget(serialButton);
    d->v1->addWidget(crcButton);

    d->v2->addWidget(aboutButton);
    d->v2->addWidget(qtButton);

    QWidget *widget = new QWidget(this);
    widget->setObjectName("menuWidget");
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addLayout(d->v1);
    layout->addLayout(d->v2);
    layout->addStretch();

    return widget;
}

QStackedWidget *MainWindow::initPageWidget(const QString& str)
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
    for(QAbstractButton* btn: d->switchButtonGroup->buttons()){
        btn->setLayoutDirection(Qt::LeftToRight);
        btn->setProperty("class", "GroupButton");
    }
    for(QAbstractButton* btn: d->menuButtonGroup->buttons()){
        btn->setProperty("class", "GroupItemButton");
        btn->setCheckable(true);
    }
    onShowGroupButton(d->switchButtonGroup->buttons().at(0));
    connect(d->switchButtonGroup, SIGNAL(buttonClicked(QAbstractButton*)),
            this, SLOT(onShowGroupButton(QAbstractButton*)));
    connect(d->menuButtonGroup, SIGNAL(buttonClicked(QAbstractButton*)),
            this, SLOT(onShowWidget(QAbstractButton*)));
}

void MainWindow::createMenuMap(const QString &name)
{
    QWidget *widget = nullptr;
    if(name == tr("Tcp Assistant"))
        widget = new TcpWidget(this);
    else if(name == tr("Serial Assistant"))
        widget = new SerialWidget(this);
    else if(name == tr("CRC Assistant"))
        widget = new CRCWidget(this);
    if(widget){
        d->menuMap.insert(name, widget);
        d->pageWidget->addWidget(widget);
        d->pageWidget->setCurrentWidget(widget);
        widget->setAttribute(Qt::WA_DeleteOnClose,true);
    }
}
