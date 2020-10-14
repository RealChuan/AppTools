#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mainwindow_global.h"

#include <QMainWindow>

class MAINWINDOW_EXPORT MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};
#endif // MAINWINDOW_H
