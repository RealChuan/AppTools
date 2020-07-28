#ifndef DRAWWIDGET_H
#define DRAWWIDGET_H

#include <QWidget>

#include "graphics_global.h"

class QListWidgetItem;
class DrawWidgetPrivate;
class GRAPHICS_EXPORT DrawWidget : public QWidget
{
public:
    DrawWidget(QWidget *parent = nullptr);
    ~DrawWidget();

private slots:
    void onAddShape(QListWidgetItem*);
    void onRemoveShape();

private:
    void setupUI();

    QScopedPointer<DrawWidgetPrivate> d;
};

#endif // DRAWWIDGET_H
