#include "mainwindow.h"
#include "plugindialog.h"
#include "configwidget.h"

#include <utils/utils.h>
#include <core/mpages.h>
#include <extensionsystem/pluginmanager.h>
#include <UserAccountSystem/useraccountsystem.h>

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

        accountButton = new QToolButton(owner);
        accountButton->setCheckable(true);
        accountButton->setObjectName("AccountButton");
        accountButton->setToolTip(QObject::tr("Account control"));
        userSystem = new UserAccountSystem(owner);
    }

    MainWindow *owner;
    QButtonGroup *switchButtonGroup;
    QButtonGroup *menuButtonGroup;
    QStackedWidget *pageWidget;
    QVBoxLayout *v1;
    QVBoxLayout *v2;
    QVBoxLayout *v3;

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

void MainWindow::onShowGroupButton(QAbstractButton *button)
{
    QVariant group = button->property("Group");
    foreach(QAbstractButton *btn, d->menuButtonGroup->buttons()){
        if(btn->property("Group") == group) btn->show();
        else btn->hide();
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
    QPushButton *aboutButton = new QPushButton(tr("About"), this);
    QPushButton *pluginButton = new QPushButton(tr("About Plugins"), this);
    QPushButton *qtButton = new QPushButton(tr("About Qt"), this);
    QPushButton *testButton = new QPushButton(tr("Test"), this);

    connect(pluginButton, &QPushButton::clicked, this, &MainWindow::onAboutPlugins);

    connect(qtButton, &QPushButton::clicked, [this]{
        QMessageBox::aboutQt(this);
    });

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
    foreach(QAbstractButton* btn, d->switchButtonGroup->buttons()){
        btn->setLayoutDirection(Qt::LeftToRight);
        btn->setProperty("class", "GroupButton");
    }
    foreach(QAbstractButton* btn, d->menuButtonGroup->buttons()){
        btn->setProperty("class", "GroupItemButton");
        btn->setCheckable(true);
    }
    onShowGroupButton(d->switchButtonGroup->buttons().at(0));
    connect(d->switchButtonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onShowGroupButton(QAbstractButton*)));
}
