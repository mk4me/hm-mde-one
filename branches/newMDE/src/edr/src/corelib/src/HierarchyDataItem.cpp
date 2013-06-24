#include "CoreLibPCH.h"
#include <corelib/HierarchyDataItem.h>
#include <corelib/HierarchyHelper.h>

using namespace core;

HierarchyDataItem::HierarchyDataItem(utils::ObjectWrapperConstPtr wrapper,  
    const QIcon& icon, const QString& name, const QString& description, HierarchyHelperPtr helper) :
    HierarchyItem(name, description, icon),
    data(wrapper)
{
    helpers.push_back(helper);
}

HierarchyDataItem::HierarchyDataItem(utils::ObjectWrapperConstPtr wrapper,  
    const QIcon& icon, const QString& name, const QString& description, std::list<HierarchyHelperPtr> helpers) :
    HierarchyItem(name, description, icon),
    helpers(helpers),
    data(wrapper)
{
}


HierarchyDataItem::HierarchyDataItem(utils::ObjectWrapperConstPtr wrapper,
    const QIcon& icon, const QString& name, const QString& description) :
    HierarchyItem(name, description, icon),
    data(wrapper)
{
    HierarchyHelperPtr helper = utils::make_shared<WrappedItemHelper>(wrapper);
    helper->setParent(this);
    helpers.push_back(helper);
}

HierarchyDataItem::HierarchyDataItem(utils::ObjectWrapperConstPtr wrapper, const QString& description, const QIcon& icon) :
    HierarchyItem( QString::fromStdString(wrapper->getClassName()), description, icon),
    data(wrapper)
{
    HierarchyHelperPtr helper = utils::make_shared<WrappedItemHelper>(wrapper);
    helper->setParent(this);
    helpers.push_back(helper);
}

HierarchyDataItem::HierarchyDataItem(const QIcon& icon, const QString& name, const QString& description, HierarchyHelperPtr helper) :
    HierarchyItem( name, description, icon)
{
    helper->setParent(this);
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
    helper->setParent(this);
    helpers.push_back(helper);
}
