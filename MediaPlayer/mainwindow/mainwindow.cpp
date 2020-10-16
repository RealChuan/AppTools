#include "mainwindow.h"
#include "playerwidget.h"

#include <utils/utils.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setCentralWidget(new PlayerWidget(this));
    resize(800, 500);
    Utils::windowCenter(this);
}

MainWindow::~MainWindow()
{
}

