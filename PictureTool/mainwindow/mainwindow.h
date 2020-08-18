#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mainwindow_global.h"

#include <controls/commonwidget.h>

using namespace Control;

class MainWindowPrivate;
class MAINWINDOW_EXPORT MainWindow : public CommonWidget
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void setupUI();
    void initToolBar();

    QScopedPointer<MainWindowPrivate> d;
};
#endif // MAINWINDOW_H
