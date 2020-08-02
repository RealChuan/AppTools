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
    QPushButton *toolsButton = initButton(tr("Common Tools"), Tool);
    QPushButton *tcpButton = initButton(tr("Tcp Assistant"), Tool);
    QPushButton *serialButton = initButton(tr("Serial Assistant"), Tool);
    QPushButton *crcButton = initButton(tr("CRC Assistant"), Tool);
    QPushButton *aboutButton = initButton(tr("About"), About);
    QPushButton *qtButton = initButton(tr("About Qt"), About);

    connect(qtButton, &QPushButton::clicked, [this]{ QMessageBox::aboutQt(this); });

    d->switchButtonGroup->addButton(toolsButton);
    d->menuButtonGroup->addButton(tcpButton);
    d->menuButtonGroup->addButton(serialButton);
    d->menuButtonGroup->addButton(crcButton);

    d->switchButtonGroup->addButton(aboutButton);
    d->menuButtonGroup->addButton(qtButton);

    initMenu();

    QWidget *widget = new QWidget(this);
    widget->setObjectName("menuWidget");
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    layout->addWidget(toolsButton);
    layout->addWidget(tcpButton);
    layout->addWidget(serialButton);
    layout->addWidget(crcButton);

    layout->addWidget(aboutButton);
    layout->addWidget(qtButton);

    layout->addStretch();

    return widget;
}

QPushButton *MainWindow::initButton(const QString &text, const MainWindow::ButtonType type)
{
    QPushButton *button = new QPushButton(text, this);
    button->setFlat(true);
    button->setProperty("Group", type);
    return button;
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
