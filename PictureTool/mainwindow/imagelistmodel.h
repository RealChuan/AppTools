#ifndef IMAGELISTMODEL_H
#define IMAGELISTMODEL_H

#include <QAbstractListModel>
#include <QListView>
#include <QPixmap>

#define WIDTH 85

struct Image{
    QString filename;
    QString absoluteFilePath;
    QPixmap pixmap;
};

typedef QVector<Image*> ImageVector;

struct ImageListModelPrivate;
class ImageListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    ImageListModel(QObject* parent = nullptr);
    ~ImageListModel();
    int columnCount(const QModelIndex & = QModelIndex()) const override{ return 1; }
    int rowCount(const QModelIndex & = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void setImageVector(const ImageVector&);

private:
    QScopedPointer<ImageListModelPrivate> d;
};

class ImageListViewPrivate;
class ImageListView : public QListView
{
    Q_OBJECT
public:
    ImageListView(QWidget* parent = nullptr);
    ~ImageListView();

    void setImageVector(const ImageVector&);

signals:
    void changeItem(int);

private slots:
    void onChangedItem(const QModelIndex &index);

private:
    void setupUI();

    QScopedPointer<ImageListViewPrivate> d;
};

#endif // IMAGELISTMODEL_H
