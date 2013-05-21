#include "MdePCH.h"
#include "AnalysisTreeItem.h"
#include <QtCore/QStringList>


AnalysisTreeItem::AnalysisTreeItem( const QString& name, const QIcon& icon) :
    icon(icon),
    name(name)
{
}

AnalysisTreeItem::~AnalysisTreeItem()
{
}

void AnalysisTreeItem::appendChild(IAnalysisTreeItemPtr item)
{
    childItems.append(item);
    item->setParent(shared_from_this());
}

IAnalysisTreeItemConstPtr AnalysisTreeItem::getChild(int row) const
{
    return childItems.value(row);
}

int AnalysisTreeItem::getNumChildren() const
{
    return childItems.count();
}


IAnalysisTreeItemConstPtr AnalysisTreeItem::getParent() const
{
    return parentItem.lock();
}

int AnalysisTreeItem::getIndex() const
{
    auto parent = parentItem.lock();
    if (parent) {
        return parent->getChildIndex(shared_from_this());
    }
    return 0;
}

QIcon AnalysisTreeItem::getIcon() const
{
    return icon;
}

QString AnalysisTreeItem::getName() const
{
    return name;
}

int AnalysisTreeItem::getChildIndex( IAnalysisTreeItemConstPtr child ) const
{
    return childItems.indexOf(utils::const_pointer_cast<IAnalysisTreeItem>(child));
}

void AnalysisTreeItem::setParent( IAnalysisTreeItemPtr parent )
{
    this->parentItem = parent;
    if (parent && parent->getChildIndex(shared_from_this()) == -1) {
        parent->appendChild(shared_from_this());
    }
}

void AnalysisTreeItem::removeChild( IAnalysisTreeItemPtr child )
{
    UTILS_ASSERT(childItems.indexOf(child) != -1);
    childItems.removeOne(child);
    child->setParent(IAnalysisTreeItemPtr());
}
