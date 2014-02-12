#include "CoreLibPCH.h"
#include <corelib/HierarchyDataItem.h>
#include <corelib/HierarchyHelper.h>

using namespace core;

HierarchyDataItem::HierarchyDataItem(utils::ObjectWrapperConstPtr wrapper,  
    const QIcon& icon, const QString& name, const QString& description, HierarchyHelperPtr helper) :
    HierarchyItem(name, description, icon),
    data(wrapper), defaultHelperIDX(0)
{
    helpers.push_back(helper);
}

HierarchyDataItem::HierarchyDataItem(utils::ObjectWrapperConstPtr wrapper,  
    const QIcon& icon, const QString& name, const QString& description, std::list<HierarchyHelperPtr> helpers) :
    HierarchyItem(name, description, icon),
    helpers(helpers),
    data(wrapper), defaultHelperIDX(-1)
{
}

core::HierarchyDataItem::HierarchyDataItem( const QIcon& icon, const QString& name, const QString& description ) :
    HierarchyItem(name, description, icon), defaultHelperIDX(-1)
{
    // helpery i data pozostaja niewypelnione danymi
    // obiekt taki nie jest gotowy do uzycia!
}


HierarchyDataItem::HierarchyDataItem(utils::ObjectWrapperConstPtr wrapper,
    const QIcon& icon, const QString& name, const QString& description) :
    HierarchyItem(name, description, icon),
    data(wrapper), defaultHelperIDX(0)
{
    HierarchyHelperPtr helper = utils::make_shared<WrappedItemHelper>(wrapper);
    helpers.push_back(helper);
}

HierarchyDataItem::HierarchyDataItem(utils::ObjectWrapperConstPtr wrapper, const QString& description, const QIcon& icon) :
    HierarchyItem( QString::fromStdString(wrapper->getClassName()), description, icon),
    data(wrapper), defaultHelperIDX(0)
{
    HierarchyHelperPtr helper = utils::make_shared<WrappedItemHelper>(wrapper);
    helpers.push_back(helper);
}

HierarchyDataItem::HierarchyDataItem(const QIcon& icon, const QString& name, const QString& description, HierarchyHelperPtr helper) :
    HierarchyItem( name, description, icon), defaultHelperIDX(0)
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

	if(defaultHelperIDX == -1){
		defaultHelperIDX = 0;
	}
}

std::list<HierarchyHelperPtr> core::HierarchyDataItem::getHelpers() const
{
    auto ptr = shared_from_this();

    for (auto it = helpers.begin(); it != helpers.end(); ++it) {
        (*it)->setParent(ptr);
    }
    

    return helpers;
}

void core::HierarchyDataItem::setDefaultHelper(const int idx)
{
	defaultHelperIDX = idx;
}

HierarchyHelperPtr core::HierarchyDataItem::getDefaultHelper() const
{
	if(defaultHelperIDX > -1){	
		auto ptr = shared_from_this();
		auto it = helpers.begin();
		std::advance(it, defaultHelperIDX);
		auto h = *it;
		h->setParent(ptr);
		return h;
	}

	return HierarchyHelperPtr();
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

