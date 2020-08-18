#include "imagelistmodel.h"

#include <QScrollBar>

struct ImageListModelPrivate{
    ImageVector imageVector;
};

ImageListModel::ImageListModel(QObject *parent)
    : QAbstractListModel(parent)
    , d(new ImageListModelPrivate)
{
}

ImageListModel::~ImageListModel()
{
}

int ImageListModel::rowCount(const QModelIndex &) const
{
    return d->imageVector.size();
}

QVariant ImageListModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();

    Image *image = d->imageVector.at(row);

    switch(role){
    case Qt::DecorationRole: return image->pixmap;
        //case Qt::DisplayRole: return image->filename;
    case Qt::ToolTipRole:return image->filename;
    case Qt::SizeHintRole: return QSize(WIDTH, WIDTH);
    case Qt::TextAlignmentRole: return Qt::AlignCenter;
    default: break;
    }

    return QVariant();
}

void ImageListModel::setImageVector(const ImageVector &imageVector)
{
    beginResetModel();
    d->imageVector = imageVector;
    endResetModel();
}

/*
 *
 * */

class ImageListViewPrivate{
public:
    ImageListViewPrivate(QWidget *parent)
        : owner(parent){
        imageListModel = new ImageListModel(owner);
    }
    QWidget *owner;
    ImageListModel *imageListModel;
};

ImageListView::ImageListView(QWidget *parent)
    : QListView(parent)
    , d(new ImageListViewPrivate(this))
{
    setupUI();
    connect(this, &QListView::doubleClicked, this, &ImageListView::onChangedItem);
}

ImageListView::~ImageListView()
{
}

void ImageListView::setImageVector(const ImageVector &imageVector)
{
    d->imageListModel->setImageVector(imageVector);
}

void ImageListView::onChangedItem(const QModelIndex &index)
{
    emit changeItem(index.row());
}

void ImageListView::setupUI()
{
    setContentsMargins(0, 0, 0, 0);
    setFlow(LeftToRight);
    setResizeMode(Adjust);
    //setViewMode(IconMode);
    setIconSize(QSize(WIDTH, WIDTH));

    //setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setModel(d->imageListModel);
}
