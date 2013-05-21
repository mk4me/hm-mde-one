#include "MdePCH.h"
#include "AnalysisTreeModel.h"
#include <QtGui/QMenu>
#include <corelib/IHierarchyDataItem.h>
#include <corelib/IHierarchyItem.h>
#include <corelib/HierarchyItem.h>
#include "AnalisisWidget.h"


AnalysisTreeModel::AnalysisTreeModel(QObject *parent) :
    QAbstractItemModel(parent)
{
    rootItem = core::IHierarchyItemPtr(new core::HierarchyItem(""));
}

//AnalysisTreeModel::AnalysisTreeModel( core::IHierarchyItemConstPtr root, QObject* parent /*= nullptr*/ ) :
//    QAbstractItemModel(parent)
//{
//    addRootItem(root);
//}

AnalysisTreeModel::~AnalysisTreeModel()
{
}

int AnalysisTreeModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant AnalysisTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    core::IHierarchyItemConstPtr item = internalSmart(index);
    if (role == Qt::DisplayRole) {
        return item->getName();
    }

    if (role == Qt::DecorationRole) {
        return item->getIcon();
    }

    return QVariant();
}

Qt::ItemFlags AnalysisTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return 0;
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant AnalysisTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}

QModelIndex AnalysisTreeModel::index(int row, int column, const QModelIndex &parent)
    const
{
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    core::IHierarchyItemConstPtr parentItem;

    if (!parent.isValid()) {
        parentItem = rootItem;
    } else {
        parentItem = internalSmart(parent);
    }

    core::IHierarchyItemConstPtr childItem = parentItem->getChild(row);
    if (childItem) {
        return createSmartIndex(row, column, childItem);
    } else {
        return QModelIndex();
    }
}

QModelIndex AnalysisTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QModelIndex();
    }

    core::IHierarchyItemConstPtr childItem = internalSmart(index);
    core::IHierarchyItemConstPtr parentItem = childItem->getParent();

    if (parentItem == rootItem) {
        return QModelIndex();
    }

    return createSmartIndex(parentItem->getIndex(), 0, parentItem);
}

int AnalysisTreeModel::rowCount(const QModelIndex &parent) const
{
    core::IHierarchyItemConstPtr parentItem;
    if (parent.column() > 0) {
        return 0;
    }

    if (!parent.isValid()) {
        parentItem = rootItem;
    } else {
        parentItem = internalSmart(parent);
    }

    return parentItem ? parentItem->getNumChildren() : 0;
}



void AnalysisTreeModel::addRootItem( core::IHierarchyItemPtr root )
{
    //raw2Smart.clear();
    rootItem->appendChild(root);

    // TODO : zrobic to wlasciwie, nie trzeba odswiezac calego drzewa
    this->reset();
}

QModelIndex AnalysisTreeModel::createSmartIndex( int row, int col, core::IHierarchyItemConstPtr ptr ) const
{
    raw2Smart[ptr.get()] = ptr;
    return createIndex(row, col, const_cast<core::IHierarchyItem*>(ptr.get()));
}

core::IHierarchyItemConstPtr AnalysisTreeModel::internalSmart( const QModelIndex& idx ) const
{
    auto raw = idx.internalPointer();
    auto it = raw2Smart.find(const_cast<void*>(raw));
    if (it != raw2Smart.end()) {
        return it->second;
    }

    UTILS_ASSERT(false);
    return core::IHierarchyItemConstPtr();
}

void AnalysisTreeModel::updateItem( core::IHierarchyItemConstPtr item )
{
    // TODO : zrobic to wlasciwie
    reset();
}

void AnalysisTreeModel::removeRootItem( core::IHierarchyItemPtr root )
{
    UTILS_ASSERT(hasChild(rootItem, root));
    // TODO : usunac wpisy z mapy...
    rootItem->removeChild(root);
}

bool AnalysisTreeModel::hasChild( core::IHierarchyItemConstPtr parent, core::IHierarchyItemConstPtr child ) const
{
    auto count = parent->getNumChildren();
    for (auto i = 0; i < count; ++i) {
        if (parent->getChild(i) == child) {
            return true;
        }
    }

    return false;
}

