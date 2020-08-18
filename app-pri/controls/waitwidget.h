#ifndef WAITWIDGET_H
#define WAITWIDGET_H

#include <QSplashScreen>

namespace Control {

class WaitWidgetPrivate;
class WaitWidget : public QSplashScreen
{
    Q_OBJECT
public:
    WaitWidget(QWidget *parent = nullptr);
    ~WaitWidget();

    void fullProgressBar();

private slots:
    void updateProgressBar();

private:
    void setupUI();

    QScopedPointer<WaitWidgetPrivate> d;
};

}

#endif // WAITWIDGET_H
