#ifndef DRAWWIDGET_H
#define DRAWWIDGET_H

#include <QWidget>

class QListWidgetItem;
class DrawWidgetPrivate;
class DrawWidget : public QWidget
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
