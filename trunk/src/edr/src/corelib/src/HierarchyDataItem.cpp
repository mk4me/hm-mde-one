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

core::HierarchyDataItem::HierarchyDataItem( const QIcon& icon, const QString& name, const QString& description ) :
    HierarchyItem(name, description, icon)
{
    // helpery i data pozostaja niewypelnione danymi
    // obiekt taki nie jest gotowy do uzycia!
}


HierarchyDataItem::HierarchyDataItem(utils::ObjectWrapperConstPtr wrapper,
    const QIcon& icon, const QString& name, const QString& description) :
    HierarchyItem(name, description, icon),
    data(wrapper)
{
    HierarchyHelperPtr helper = utils::make_shared<WrappedItemHelper>(wrapper);
    helpers.push_back(helper);
}

HierarchyDataItem::HierarchyDataItem(utils::ObjectWrapperConstPtr wrapper, const QString& description, const QIcon& icon) :
    HierarchyItem( QString::fromStdString(wrapper->getClassName()), description, icon),
    data(wrapper)
{
    HierarchyHelperPtr helper = utils::make_shared<WrappedItemHelper>(wrapper);
    helpers.push_back(helper);
}

HierarchyDataItem::HierarchyDataItem(const QIcon& icon, const QString& name, const QString& description, HierarchyHelperPtr helper) :
    HierarchyItem( name, description, icon)
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
    helper->setParent(HierarchyItem::shared_from_this());
    helpers.push_back(helper);
}

std::list<HierarchyHelperPtr> core::HierarchyDataItem::getHelpers() const
{
    auto ptr = shared_from_this();

    for (auto it = helpers.begin(); it != helpers.end(); ++it) {
        (*it)->setParent(ptr);
    }
    

    return helpers;
}

core::IHierarchyItemPtr core::HierarchyDataItem::shallowCopy(bool withChildren) const
{
    auto hi = utils::make_shared<core::HierarchyDataItem>(*this);
    hi->childItems.clear();
    if (withChildren) {
        for (auto it = childItems.begin(); it != childItems.end(); ++it) {
            hi->appendChild((*it)->shallowCopy());
        }
    }
    return hi;
}

