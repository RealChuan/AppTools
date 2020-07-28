#include "mainwindow.h"
#include "imageviewer.h"

#include <graphics/drawwidget.h>
#include <utils/utils.h>

#include <QtWidgets>

class MainWindowPrivate{
public:
    MainWindowPrivate(QWidget *parent)
        : owner(parent){}
    QWidget *owner;
};

MainWindow::MainWindow(QWidget *parent) : CommonWidget(parent)
  , d(new MainWindowPrivate(this))
{
    setTitle(tr("PictureTool"));
    connect(this, &MainWindow::aboutToclose, qApp, &QApplication::quit);
    //setCentralWidget(new ImageViewer(this));
    setCentralWidget(new DrawWidget(this));
    initToolBar();
    resize(1000, 600);
}

MainWindow::~MainWindow()
{

}

void MainWindow::initToolBar()
{
    QPushButton *qssButton = new QPushButton(tr("Reload QSS"), this);
    qssButton->setObjectName("QssButton");
    connect(qssButton, &QPushButton::clicked, this, &Utils::setQSS);

    QWidget *titleBar = new QWidget(this);
    QHBoxLayout *titleLayout = new QHBoxLayout(titleBar);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->setSpacing(10);
    titleLayout->addWidget(qssButton);
    setTitleBar(titleBar);
}

