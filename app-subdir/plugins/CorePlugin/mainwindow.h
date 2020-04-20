#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <controls/commonwidget.h>

class QStackedWidget;
class QAbstractButton;
class MainWindowPrivate;
class MainWindow : public CommonWidget
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void extensionsInitialized();

private slots:
    void onShowGroupButton(QAbstractButton*);
    void onAboutPlugins();
    void onAccount();

private:
    void setupUI();
    void initToolBar();
    QWidget* menuWidget();
    QStackedWidget* initPageWidget(QString);
    void initMenu();

    MainWindowPrivate *d;
};
#endif // MAINWINDOW_H
