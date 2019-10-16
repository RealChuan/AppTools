#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QListWidget>
#include <QMainWindow>
#include <QSizeGrip>
#include <QTreeWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

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
    void titleBtn();

    //QListWidget菜单
    void initListWidget();
    void on_listWidget_clicked(const QModelIndex &);
    void createMenuMap(QString className);

    //QTreeWidget菜单
    //void initTreeWidget();
    //void on_treeWidget_clicked(const QModelIndex &index);

    void initMenuBtnWidget();

private:
    Ui::MainWindow *ui;
    //记录鼠标位置
    QPoint lastPoint;
    QPoint movePoint;
    //右下角缩放功能
    QSizeGrip *sizeGrip;
    //界面map
    QMap<QString,QWidget*> menuMap;

};
#endif // MAINWINDOW_H
