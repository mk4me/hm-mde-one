#include "DicomPCH.h"

#include "LayeredModelView.h"
#include "Adnotations.h"

using namespace dicom; 

LayeredModelView::LayeredModelView(QObject *parent)
    :QAbstractTableModel(parent)
{
}

//-----------------------------------------------------------------
int LayeredModelView::rowCount(const QModelIndex & /*parent*/) const
{
    return image ? image->getNumLayers() : 0;
}

//-----------------------------------------------------------------
int LayeredModelView::columnCount(const QModelIndex & /*parent*/) const
{
    return 2;
}

//-----------------------------------------------------------------
QVariant LayeredModelView::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole && image)
    {
        ILayerItemConstPtr itm = image->getLayer(index.row());
        if (index.column() == 0) {
            return itm->getName();
        } else if (index.column() == 1) {
            adnotations::AdnotationsTypePtr adn = adnotations::instance();
            auto it = adn->left.find(itm->getAdnotationIdx());
            if (it != adn->left.end()) {
                return it->second;
            }
        }
    }
    return QVariant();
}

//-----------------------------------------------------------------
//! [quoting mymodel_e]
bool LayeredModelView::setData(const QModelIndex & index, const QVariant & value, int role)
{
    if (role == Qt::EditRole)
    {
        int idx = index.row();
        int col = index.column();
        if (image->getNumLayers() > idx && idx >= 0) {
            if (col == 0) {
                auto result = value.toString();
                if (!result.isEmpty()) {
                    image->getLayer(idx)->setName(result);
                    Q_EMIT editCompleted( result );
                }
            } else if (col == 1) {
                QString result = value.toString();
                auto adn = adnotations::instance();
                int adnotationIdx = adn->right.at(result);
                image->getLayer(idx)->setAdnotationIdx(adnotationIdx);
                Q_EMIT editCompleted( result );
            }
        }
        
    }
    return true;
}
//! [quoting mymodel_e]

//-----------------------------------------------------------------
//! [quoting mymodel_f]
Qt::ItemFlags LayeredModelView::flags(const QModelIndex & /*index*/) const
{
    return Qt::ItemIsSelectable |  Qt::ItemIsEditable | Qt::ItemIsEnabled ;
}

QVariant LayeredModelView::headerData( int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole */ ) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            case 0:
                return QString("Name");
            case 1:
                return QString("Adnotation");
            }
        }
    }
    return QVariant();
}

dicom::ILayeredImageConstPtr dicom::LayeredModelView::getImage() const
{
    return image;
}

void dicom::LayeredModelView::setImage( ILayeredImagePtr val )
{
    image = val;
    this->reset();
}

//! [quoting mymodel_f]
