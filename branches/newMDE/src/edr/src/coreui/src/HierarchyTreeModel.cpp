#include "CoreUiPCH.h"
#include <coreui/HierarchyTreeModel.h>
#include <QtGui/QMenu>
#include <corelib/IHierarchyDataItem.h>
#include <corelib/IHierarchyItem.h>
#include <corelib/HierarchyItem.h>


using namespace coreui;

HierarchyTreeModel::HierarchyTreeModel(QObject *parent) :
    QAbstractItemModel(parent)
{
    rootItem = core::IHierarchyItemPtr(new core::HierarchyItem("",""));
}

//HierarchyTreeModel::HierarchyTreeModel( core::IHierarchyItemConstPtr root, QObject* parent /*= nullptr*/ ) :
//    QAbstractItemModel(parent)
//{
//    addRootItem(root);
//}

HierarchyTreeModel::~HierarchyTreeModel()
{
}

int HierarchyTreeModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant HierarchyTreeModel::data(const QModelIndex &index, int role) const
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

Qt::ItemFlags HierarchyTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return 0;
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant HierarchyTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}

QModelIndex HierarchyTreeModel::index(int row, int column, const QModelIndex &parent)
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

QModelIndex HierarchyTreeModel::parent(const QModelIndex &index) const
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

int HierarchyTreeModel::rowCount(const QModelIndex &parent) const
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



void HierarchyTreeModel::addRootItem( core::IHierarchyItemConstPtr root )
{
    //raw2Smart.clear();
    rootItem->appendChild(root);

    // TODO : zrobic to wlasciwie, nie trzeba odswiezac calego drzewa
    this->reset();
}

QModelIndex HierarchyTreeModel::createSmartIndex( int row, int col, core::IHierarchyItemConstPtr ptr ) const
{
    raw2Smart[ptr.get()] = ptr;
    return createIndex(row, col, const_cast<core::IHierarchyItem*>(ptr.get()));
}

core::IHierarchyItemConstPtr HierarchyTreeModel::internalSmart( const QModelIndex& idx ) const
{
    if (idx.isValid()) {
        auto raw = idx.internalPointer();
        auto it = raw2Smart.find(const_cast<void*>(raw));
        if (it != raw2Smart.end()) {
            return it->second;
        }

        UTILS_ASSERT(false);
    }
    return core::IHierarchyItemConstPtr();
}

void HierarchyTreeModel::updateItem( core::IHierarchyItemConstPtr item )
{
    // TODO : zrobic to wlasciwie
    reset();
}

void HierarchyTreeModel::removeRootItem( core::IHierarchyItemConstPtr root )
{
    UTILS_ASSERT(hasChild(rootItem, root));
    // TODO : usunac wpisy z mapy...
    rootItem->removeChild(root);
}

bool HierarchyTreeModel::hasChild( core::IHierarchyItemConstPtr parent, core::IHierarchyItemConstPtr child ) const
{
    auto count = parent->getNumChildren();
    for (auto i = 0; i < count; ++i) {
        if (parent->getChild(i) == child) {
            return true;
        }
    }

    return false;
}


void coreui::HierarchyTreeModel::clear()
{
    auto count = rootItem->getNumChildren();
    for (auto i = count - 1; i >= 0; --i) {
        auto child = rootItem->getChild(i);
        rootItem->removeChild(child);
    }
}

void coreui::HierarchyTreeModel::applyChange( const core::IMemoryDataManagerHierarchy::HierarchyChange& change )
{
    switch(change.modification) {
    case core::IDataManagerReader::ADD_OBJECT:
        addRootItem(change.value);
        break;
    case  core::IDataManagerReader::REMOVE_OBJECT:
        removeRootItem(change.value);
        break;
    case core::IDataManagerReader::UPDATE_OBJECT:
        updateItem(change.value);
        break;
    default:
        UTILS_ASSERT(false);
    }
}

void coreui::HierarchyTreeModel::applyChanges( const core::IMemoryDataManagerHierarchy::HierarchyChangeList & changes )
{
    for (auto it = changes.begin(); it != changes.end(); ++it) {
        applyChange(*it);
    }
}
