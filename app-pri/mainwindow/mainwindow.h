#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <controls/commonwidget.h>

class QPushButton;
class QStackedWidget;
class QAbstractButton;
class MainWindowPrivate;
class MainWindow : public CommonWidget
{
    Q_OBJECT
public:
    enum ButtonType{
        Tool = 1,
        About
    };

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void onAccount(bool);

private slots:
    void onShowGroupButton(QAbstractButton*);
    void onShowWidget(QAbstractButton*);

private:
    void setupUI();
    void initToolBar();
    QWidget* menuWidget();
    QPushButton *initButton(const QString &text, const ButtonType type);
    QStackedWidget* initPageWidget(const QString&);
    void initMenu();
    void createMenuMap(const QString& name);

    QScopedPointer<MainWindowPrivate> d;
};
#endif // MAINWINDOW_H
