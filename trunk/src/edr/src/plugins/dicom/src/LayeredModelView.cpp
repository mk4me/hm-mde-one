#include "DicomPCH.h"

#include "LayeredModelView.h"
#include "Adnotations.h"
#include <QtGui/QFont>

using namespace dicom; 

LayeredModelView::LayeredModelView(QObject *parent)
    :QAbstractItemModel(parent)
{
}

//-----------------------------------------------------------------
int LayeredModelView::rowCount(const QModelIndex & parent) const
{
    if (!parent.isValid()) {
        return image->getNumTags();
    } else {
        treedata* td = static_cast<treedata*>(parent.internalPointer());
        if (td->idx == -1) {
            std::string tag = image->getTag(td->tag);
            return image->getNumLayerItems(tag);
        } else {
            return 0;
        }
    }
}

//-----------------------------------------------------------------
int LayeredModelView::columnCount(const QModelIndex & parent) const
{
    return 2;
    //return parent.isValid() ? 2 : 1;
}

//-----------------------------------------------------------------
QVariant LayeredModelView::data(const QModelIndex &index, int role) const
{
    if (image) {
        if ( role == Qt::CheckStateRole && index.column() == 0 ) {
            treedata* td = static_cast<treedata*>(index.internalPointer());
            if (td->idx == -1) {
                std::string tag = image->getTag(td->tag);
                return static_cast< int >(image->getTagVisible(tag) ? Qt::Checked : Qt::Unchecked);
            }
        }
        if (role == Qt::DisplayRole) {
            treedata* td = static_cast<treedata*>(index.internalPointer());
            std::string tag = image->getTag(td->tag);
            if (td->idx == -1) {
                if (index.column() == 0) {
                    return QString::fromStdString(tag);
                }
            } else {
                ILayerItemConstPtr itm = image->getLayerItem(tag, index.row());
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
            
        } else if (role == Qt::FontRole) {
                treedata* td = static_cast<treedata*>(index.internalPointer());
            if (td->idx != -1) {
                std::string tag = image->getTag(td->tag);
                ILayerItemConstPtr itm = image->getLayerItem(tag, index.row());
                QFont f;
                f.setBold(itm->getSelected());
                return f;
            }
        }
    }
    
    return QVariant();
}

//-----------------------------------------------------------------
//! [quoting mymodel_e]
bool LayeredModelView::setData(const QModelIndex & index, const QVariant & value, int role)
{
    if ( role == Qt::CheckStateRole) {
        treedata* td = static_cast<treedata*>(index.internalPointer());
        if (td->idx == -1) {
            Qt::CheckState checked = static_cast< Qt::CheckState >( value.toInt() );
            std::string tag = image->getTag(td->tag);
            image->setTagVisible(tag, checked == Qt::Checked);
        }

    }
    if (role == Qt::EditRole)
    {
        treedata* td = static_cast<treedata*>(index.internalPointer());
        int idx = td->idx;
        if (idx != -1) {
            std::string tag = image->getTag(td->tag);
            int col = index.column();
            if (image->getNumLayerItems(tag) > idx && idx >= 0) {
                if (col == 0) {
                    auto result = value.toString();
                    if (!result.isEmpty()) {
                        image->getLayerItem(tag, idx)->setName(result);
                        Q_EMIT editCompleted( result );
                    }
                } else if (col == 1) {
                    QString result = value.toString();
                    auto adn = adnotations::instance();
                    int adnotationIdx = adn->right.at(result);
                    image->getLayerItem(tag, idx)->setAdnotationIdx(adnotationIdx);
                    Q_EMIT editCompleted( result );
                }
            }
        }
    }
    return true;
}
//! [quoting mymodel_e]

//-----------------------------------------------------------------
//! [quoting mymodel_f]
Qt::ItemFlags LayeredModelView::flags(const QModelIndex & index) const
{
    if (!index.isValid()) {
        return 0;
    }

    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    treedata* td = static_cast<treedata*>(index.internalPointer());
    if (td->idx == -1) {
        if ( index.column() == 0 ) {
            flags |= Qt::ItemIsUserCheckable;
        }
    } else {
        flags |=  Qt::ItemIsEditable ;
    }

    return flags;
}

QVariant LayeredModelView::headerData( int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole */ ) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            case 0:
                return QString("Tag");
            case 1:
                return QString("Label");
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

bool dicom::LayeredModelView::removeRows( int row, int count, const QModelIndex &parent /*= QModelIndex( ) */ )
{
    std::list<ILayerItemConstPtr> toRemove;
    // TODO
    /*int endRow = (std::min)(row + count, image->getNumLayerItems(TODO));
    for (int i = row; i < endRow; ++i) {
        toRemove.push_back(image->getLayerItem(i));
    }
    
    for (auto it = toRemove.begin(); it != toRemove.end(); ++it) {
        image->removeLayer(*it);
    }*/

    return !toRemove.empty();
}

QModelIndex dicom::LayeredModelView::index( int row, int column, const QModelIndex &parent /*= QModelIndex( ) */ ) const
{
    if (!parent.isValid()) {
        if (row >= 0 && row < image->getNumTags()) {
            return createIndex(row, column, (void*)getData(row, -1));
        } else {
            return QModelIndex();
        }
    } else {
        treedata* td = static_cast<treedata*>(parent.internalPointer());
        if (td->idx == -1) {
            return createIndex(row, column, (void*)getData(td->tag, row));
        } else {
            return QModelIndex();
        }
    }


}

QModelIndex dicom::LayeredModelView::parent( const QModelIndex &child ) const
{
    if (child.isValid()) {
        treedata* td = static_cast<treedata*>(child.internalPointer());
        if (td->idx != -1) {
            return createIndex(td->tag, 0, (void*)getData(td->tag, -1));
        }
    }
    return QModelIndex();
}

const dicom::LayeredModelView::treedata* dicom::LayeredModelView::getData( int tag, int idx ) const
{
    // insert zwraca pare <iterator do elementu, flaga okreslajaca czy element isnial czy wlasnie zostal dodany> 
    treedata t = {tag, idx};
    return &(*(datas.insert(t).first));
}

std::pair<int, int> dicom::LayeredModelView::getTagAndIndex( const QModelIndex& idx )
{
    if (idx.isValid()) {
        treedata* td = static_cast<treedata*>(idx.internalPointer());
        if (td) {
            return std::make_pair(td->tag, td->idx);
        }
    }

    return std::make_pair(-1, -1);
}

void dicom::LayeredModelView::refreshSelected()
{
    Q_EMIT layoutChanged();
}

dicom::LayeredModelView::Expands dicom::LayeredModelView::getExpands() const
{
    Expands exp;
    for (int row = image->getNumTags() - 1; row >= 0; --row) {
        exp.push_back(std::make_pair(createIndex(row, 0, (void*)getData(row, -1)), image->getTagVisible(image->getTag(row))));
    }
    return exp;
}

//! [quoting mymodel_f]
