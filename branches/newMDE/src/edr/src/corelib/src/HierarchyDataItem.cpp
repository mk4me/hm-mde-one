#include "CoreLibPCH.h"
#include <corelib/HierarchyDataItem.h>
#include <corelib/HierarchyHelper.h>

using namespace core;

HierarchyDataItem::HierarchyDataItem(utils::ObjectWrapperConstPtr wrapper,  
    const QIcon& icon, const QString& name, HierarchyHelperPtr helper) :
    HierarchyItem(name, icon),
    data(wrapper)
{
    helpers.push_back(helper);
}

HierarchyDataItem::HierarchyDataItem(utils::ObjectWrapperConstPtr wrapper,  
    const QIcon& icon, const QString& name, std::list<HierarchyHelperPtr> helpers) :
    HierarchyItem(name, icon),
    helpers(helpers),
    data(wrapper)
{
}


HierarchyDataItem::HierarchyDataItem(utils::ObjectWrapperConstPtr wrapper,
    const QIcon& icon, const QString& name) :
    HierarchyItem(name, icon),
    data(wrapper)
{
    helpers.push_back(HierarchyHelperPtr(new WrappedItemHelper(wrapper)));
}

HierarchyDataItem::HierarchyDataItem(utils::ObjectWrapperConstPtr wrapper, const QIcon& icon) :
    HierarchyItem( QString::fromStdString(wrapper->getClassName()), icon),
    data(wrapper)
{
    helpers.push_back(HierarchyHelperPtr(new WrappedItemHelper(wrapper)));
}

HierarchyDataItem::HierarchyDataItem(const QIcon& icon, const QString& name, HierarchyHelperPtr helper) :
    HierarchyItem( name, icon)
{
    helpers.push_back(helper);
}

HierarchyDataItem::~HierarchyDataItem()
{
}

utils::ObjectWrapperConstPtr core::HierarchyDataItem::getData() const
{
    return data;
}

void core::HierarchyDataItem::addHelper( HierarchyHelperPtr helper )
{
    helpers.push_back(helper);
}
