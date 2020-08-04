#ifndef SYSTEMCONVERSIONBOX_H
#define SYSTEMCONVERSIONBOX_H

#include <QGroupBox>

class SystemConversionBoxPrivate;
class SystemConversionBox : public QGroupBox
{
    Q_OBJECT
public:
    SystemConversionBox(QWidget *parent = nullptr);
    ~SystemConversionBox();

private slots:
    void onDecimalChanged(const QString&);
    void onHexChanged(const QString&);

private:
    void setupUI();
    void buildConnect();

    QScopedPointer<SystemConversionBoxPrivate> d;
};

#endif // SYSTEMCONVERSIONBOX_H
