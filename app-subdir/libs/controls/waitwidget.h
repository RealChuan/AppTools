#ifndef WAITWIDGET_H
#define WAITWIDGET_H

#include "controls_global.h"

#include <QSplashScreen>

class WaitWidgetPrivate;
class CONTROLS_EXPORT WaitWidget : public QSplashScreen
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

    WaitWidgetPrivate *d;
};

#endif // WAITWIDGET_H
