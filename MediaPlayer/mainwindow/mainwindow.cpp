#include "mainwindow.h"
#include "playerwidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setCentralWidget(new PlayerWidget(this));
    resize(800, 640);
}

MainWindow::~MainWindow()
{
}

