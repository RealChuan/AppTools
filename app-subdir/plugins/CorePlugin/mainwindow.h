#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QStackedWidget;
class QAbstractButton;
class MainWindowPrivate;
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void extensionsInitialized();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private slots:
    void showGroupButton(QAbstractButton*);
    void aboutPlugins();

private:
    void setupUI();
    QWidget* titleWidget();
    QWidget* menuWidget();
    QStackedWidget* initPageWidget(QString);
    void initMenu();

    MainWindowPrivate *d;
};
#endif // MAINWINDOW_H
