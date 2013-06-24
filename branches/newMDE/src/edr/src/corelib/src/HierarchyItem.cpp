#include "CoreLibPCH.h"
#include <corelib/HierarchyItem.h>
#include <QtCore/QStringList>

using namespace core;

HierarchyItem::HierarchyItem( const QString& name, const QString& description, const QIcon& icon) :
    icon(icon),
    name(name),
    desc(description)
{
}

HierarchyItem::~HierarchyItem()
{
}

void HierarchyItem::appendChild(IHierarchyItemConstPtr item)
{
    childItems.append(item);
    IHierarchyItemPtr ptr = utils::const_pointer_cast<IHierarchyItem>(item);
    ptr->setParent(shared_from_this());
}

IHierarchyItemConstPtr HierarchyItem::getChild(int row) const
{
    return childItems.value(row);
}

int HierarchyItem::getNumChildren() const
{
    return childItems.count();
}


IHierarchyItemConstPtr HierarchyItem::getParent() const
{
    return parentItem.lock();
}

int HierarchyItem::getIndex() const
{
    auto parent = parentItem.lock();
    if (parent) {
        return parent->getChildIndex(shared_from_this());
    }
    return 0;
}

QIcon HierarchyItem::getIcon() const
{
    return icon;
}

QString HierarchyItem::getName() const
{
    return name;
}

int HierarchyItem::getChildIndex( IHierarchyItemConstPtr child ) const
{
    return childItems.indexOf(utils::const_pointer_cast<IHierarchyItem>(child));
}

void HierarchyItem::setParent( IHierarchyItemConstPtr parent )
{
    this->parentItem = parent;
    if (parent && parent->getChildIndex(shared_from_this()) == -1) {
        auto ptr = utils::const_pointer_cast<IHierarchyItem>(parent);
        ptr->appendChild(shared_from_this());
    }
}

void HierarchyItem::removeChild( IHierarchyItemConstPtr child )
{
    UTILS_ASSERT(childItems.indexOf(child) != -1);
    childItems.removeOne(child);
    auto ptr = utils::const_pointer_cast<IHierarchyItem>(child);
    ptr->setParent(IHierarchyItemConstPtr());
}

QString core::HierarchyItem::getDescription() const
{
    return desc;
}
