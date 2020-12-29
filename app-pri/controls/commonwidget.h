#ifndef COMMONWIDGET_H
#define COMMONWIDGET_H

#include <QWidget>

namespace Control {

class CommonWidgetPrivate;
class CommonWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CommonWidget(QWidget *parent = nullptr);
    ~CommonWidget();

    void setRestoreMaxButtonVisible(bool);
    void setMinButtonVisible(bool);

    void setTitle(const QString&);
    void setIcon(const QIcon&);

    void setCentralWidget(QWidget*);
    void setTitleBar(QWidget*);

    void setShadowPadding(int shadowPadding = 10);

signals:
    void aboutToclose();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

private:
    void setupUI();
    QWidget* titleWidget();
    void buildConnnect();

    QScopedPointer<CommonWidgetPrivate> d;
};

}

#endif // COMMONWIDGET_H
