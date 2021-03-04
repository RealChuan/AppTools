#ifndef COMMONWIDGET_H
#define COMMONWIDGET_H

#include "controls_global.h"

#include <QWidget>

namespace Control {

class CommonWidgetPrivate;
class CONTROLS_EXPORT CommonWidget : public QWidget
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

private slots:
    void onShowMaximized();
    void onShowNormal();

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
