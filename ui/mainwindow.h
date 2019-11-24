#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QListWidget>
#include <QMainWindow>
#include <QSizeGrip>
#include <QTreeWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindowPrivate;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:   
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);

    void setPaddingAndSpacing();
    void initWindow();
    void initMenuButton();
    void initListButton();
    void titleBtn();
    void createMenuMap(QString className);

private:
    Ui::MainWindow *ui;
    MainWindowPrivate *d;
};
#endif // MAINWINDOW_H
