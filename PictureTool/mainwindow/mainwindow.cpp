#include "mainwindow.h"
#include "imageviewer.h"
#include "drawwidget.h"

#include <utils/utils.h>

#include <QtWidgets>

class MainWindowPrivate{
public:
    MainWindowPrivate(QWidget *parent)
        : owner(parent){
        drawWidget = new DrawWidget(owner);
        imageViewer = new ImageViewer(owner);
    }
    QWidget *owner;
    DrawWidget *drawWidget;
    ImageViewer *imageViewer;
};

MainWindow::MainWindow(QWidget *parent) : CommonWidget(parent)
  , d(new MainWindowPrivate(this))
{
    connect(this, &MainWindow::aboutToclose, qApp, &QApplication::quit);
    setupUI();
    resize(1000, 600);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    setTitle(tr("PictureTool"));
    initToolBar();
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(d->imageViewer);
    layout->addWidget(d->drawWidget);
    d->drawWidget->hide();
    setCentralWidget(widget);
}

void MainWindow::initToolBar()
{
    QPushButton *qssButton = new QPushButton(tr("Reload QSS"), this);
    qssButton->setObjectName("QssButton");
    connect(qssButton, &QPushButton::clicked, this, &Utils::setQSS);

    QMenu *menu = new QMenu(tr("Mainwindow"), this);
    menu->addAction(tr("DrawWidget"), [this] {
        d->drawWidget->show();
        d->imageViewer->hide();
    });
    menu->addSeparator();
    menu->addAction(tr("ImageView"), [this] {
        d->imageViewer->show();
        d->drawWidget->hide();
    });
    QMenuBar *menuBar = new QMenuBar(this);
    menuBar->addMenu(menu);

    QWidget *titleBar = new QWidget(this);
    QHBoxLayout *titleLayout = new QHBoxLayout(titleBar);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->setSpacing(10);
    titleLayout->addWidget(menuBar);
    titleLayout->addWidget(qssButton);
    setTitleBar(titleBar);
}
