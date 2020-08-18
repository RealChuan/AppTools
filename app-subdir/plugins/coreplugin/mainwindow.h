#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <controls/commonwidget.h>

using namespace Control;

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

public slots:
    void onAccount(bool);

private slots:
    void onShowGroupButton(QAbstractButton*);
    void onAboutPlugins();

private:
    void setupUI();
    void initToolBar();
    QWidget* menuWidget();
    QStackedWidget* initPageWidget(QString);
    void initMenu();

    QScopedPointer<MainWindowPrivate> d;
};
#endif // MAINWINDOW_H
