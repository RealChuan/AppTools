#ifndef MPAGES_H
#define MPAGES_H

#include "core_global.h"

#include <QObject>

class QPushButton;

namespace Core {

class MPagesPrivate;
class CORE_EXPORT MPages : public QObject
{
    Q_OBJECT
public:
    enum Group{ Tool, About, Test };

    explicit MPages(QObject *parent = nullptr);
    ~MPages();

    QPushButton* button() const;
    QWidget* widget() const;

protected:
    void setWidget(QWidget*);
    void setButtonName(const QString&);
    void setButtonGroup(Group);
    void setButtonObejctName(const QString& objectname);

private:
    QScopedPointer<MPagesPrivate> d;

};

}

#endif // MPAGES_H
