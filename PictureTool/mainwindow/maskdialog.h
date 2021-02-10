#ifndef MASKDIALOG_H
#define MASKDIALOG_H

#include <controls/dialog.h>

using namespace Control;

class MaskDialogPrivate;
class MaskDialog : public Dialog
{
public:
    MaskDialog(QWidget *parent = nullptr);
    ~MaskDialog();

    void setImageName(const QString &name);

    void setPixmap(const QPixmap& pixmap);
    QImage maskImage();
    void setMaskImage(const QImage& image);

private slots:
    void onButtonClicked(int id);
    void onPenSizeChanged(int value);
    void onOpacityChanged(double value);
    void onSave();

private:
    void setupUI();
    void buildConnect();

    QScopedPointer<MaskDialogPrivate> d_ptr;
};

#endif // MASKDIALOG_H
