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
        stackedWidget = new QStackedWidget(owner);
        stackedWidget->addWidget(imageViewer);
        stackedWidget->addWidget(drawWidget);
    }
    QWidget *owner;
    DrawWidget *drawWidget;
    ImageViewer *imageViewer;
    QStackedWidget *stackedWidget;
};

MainWindow::MainWindow(QWidget *parent)
    : CommonWidget(parent)
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
    setCentralWidget(d->stackedWidget);
}

void MainWindow::initToolBar()
{
    QPushButton *qssButton = new QPushButton(tr("Reload QSS"), this);
    qssButton->setObjectName("QssButton");
    connect(qssButton, &QPushButton::clicked, this, &Utils::setQSS);

    QComboBox *menuBox = new QComboBox(this);
    menuBox->setObjectName("MenuBox");
    menuBox->addItems(QStringList() << tr("ImageViewer") << tr("DrawWidget"));
    connect(menuBox, &QComboBox::currentTextChanged,
            [this](const QString &text){
        if(text == tr("ImageViewer"))
            d->stackedWidget->setCurrentWidget(d->imageViewer);
        else if(text == tr("DrawWidget"))
            d->stackedWidget->setCurrentWidget(d->drawWidget);
    });

    QWidget *titleBar = new QWidget(this);
    QHBoxLayout *titleLayout = new QHBoxLayout(titleBar);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->setSpacing(10);
    titleLayout->addWidget(menuBox);
    titleLayout->addWidget(qssButton);
    setTitleBar(titleBar);
}
