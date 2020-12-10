#include "mainwindow.h"
#include "plugindialog.h"
#include "configwidget.h"

#include <utils/utils.h>
#include <core/mpages.h>
#include <extensionsystem/pluginmanager.h>
#include <useraccountsystem/useraccountsystem.h>
#include <controls/messbox.h>

#include <QtWidgets>

using namespace ExtensionSystem;
using namespace AccountSystem;
using namespace Core;

class MainWindowPrivate{
public:
    MainWindowPrivate(MainWindow *parent)
        : owner(parent){
        switchButtonGroup = new QButtonGroup(owner);
        switchButtonGroup->setExclusive(true);
        menuButtonGroup = new QButtonGroup(owner);
        menuButtonGroup->setExclusive(true);
        pageWidget = new QStackedWidget(owner);

        vLayoutGroup1 = new QVBoxLayout;
        vLayoutGroup2 = new QVBoxLayout;
        vLayoutGroup3 = new QVBoxLayout;
        vLayoutGroup1->setContentsMargins(0, 0, 0, 0);
        vLayoutGroup1->setSpacing(0);
        vLayoutGroup2->setContentsMargins(0, 0, 0, 0);
        vLayoutGroup2->setSpacing(0);
        vLayoutGroup3->setContentsMargins(0, 0, 0, 0);
        vLayoutGroup3->setSpacing(0);

        accountButton = new QToolButton(owner);
        accountButton->setCheckable(true);
        accountButton->setObjectName("AccountButton");
        accountButton->setToolTip(QObject::tr("Account control"));
        userSystem = new UserAccountSystem(owner);
        userSystem->setVisible(false);
        accountButton->setChecked(userSystem->loginState());
    }

    MainWindow *owner;
    QButtonGroup *switchButtonGroup;
    QButtonGroup *menuButtonGroup;
    QStackedWidget *pageWidget;
    QVBoxLayout *vLayoutGroup1;
    QVBoxLayout *vLayoutGroup2;
    QVBoxLayout *vLayoutGroup3;

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

void MainWindow::extensionsInitialized()
{
    for(const MPages* page: PluginManager::getObjects<MPages>()){
        if(!page->widget())
            continue;
        if(page->button()->property("Group") == MPages::Tool)
            d->vLayoutGroup1->addWidget(page->button());
        else if(page->button()->property("Group") == MPages::About)
            d->vLayoutGroup2->addWidget(page->button());
        else if(page->button()->property("Group") == MPages::Test)
            d->vLayoutGroup3->addWidget(page->button());
        else
            continue;
        d->menuButtonGroup->addButton(page->button());
        d->pageWidget->addWidget(page->widget());
        connect(page->button(), &QPushButton::clicked, [=]{
            d->pageWidget->setCurrentWidget(page->widget());
        });
    }

    initMenu();
}

void MainWindow::onShowGroupButton(QAbstractButton *button)
{
    QVariant group = button->property("Group");
    foreach(QAbstractButton *btn, d->menuButtonGroup->buttons()){
        if(btn->property("Group") == group)
            btn->show();
        else
            btn->hide();
    }
}

void MainWindow::onAboutPlugins()
{
    Core::Internal::PluginDialog dialog(this);
    dialog.exec();
}

void MainWindow::onAccount(bool state)
{
    d->accountButton->setChecked(!state);
    d->userSystem->exec();
}

void MainWindow::setupUI()
{
    initToolBar();
    createTray();

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
    connect(d->userSystem, &UserAccountSystem::login, d->accountButton, &QToolButton::setChecked);

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
    toolsButton->setObjectName("ToolGroupButton");
    QPushButton *aboutButton = new QPushButton(tr("About"), this);
    aboutButton->setObjectName("AbortGroupButton");
    QPushButton *pluginButton = new QPushButton(tr("About Plugins"), this);
    pluginButton->setObjectName("PluginsButton");
    QPushButton *qtButton = new QPushButton(tr("About Qt"), this);
    qtButton->setObjectName("QtButton");
    QPushButton *testButton = new QPushButton(tr("Test"), this);
    testButton->setObjectName("TestGroupButton");

    connect(pluginButton, &QPushButton::clicked, this, &MainWindow::onAboutPlugins);
    connect(qtButton, &QPushButton::clicked, [this]{ QMessageBox::aboutQt(this); });

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

    d->vLayoutGroup1->addWidget(toolsButton);

    d->vLayoutGroup2->addWidget(aboutButton);
    d->vLayoutGroup2->addWidget(pluginButton);
    d->vLayoutGroup2->addWidget(qtButton);

    d->vLayoutGroup3->addWidget(testButton);

    QWidget *widget = new QWidget(this);
    widget->setObjectName("menuWidget");
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addLayout(d->vLayoutGroup1);
    layout->addLayout(d->vLayoutGroup2);
    layout->addLayout(d->vLayoutGroup3);
    layout->addStretch();

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
}

void MainWindow::createTray()
{
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        MessBox::Info(this, tr("Systray, I couldn't detect any system "
                               "tray on this system."));
        return;
    }

    QMenu *menu = new QMenu(this);
    menu->addAction(tr("Quit"), qApp, &QApplication::quit);

    QSystemTrayIcon *systemTrayIcon = new QSystemTrayIcon(this);
    systemTrayIcon->setToolTip(tr("This is an app."));
    systemTrayIcon->setIcon(QIcon(":/icon/app.ico"));
    systemTrayIcon->setContextMenu(menu);
    systemTrayIcon->show();

    qApp->setQuitOnLastWindowClosed(false);
}
