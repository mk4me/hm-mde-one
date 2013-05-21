/********************************************************************
	created:	2013/05/07
	created:	7:5:2013   14:28
	filename: 	HierarchyDataItem.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__HIERARCHYDATAITEM_H__
#define HEADER_GUARD_CORE__HIERARCHYDATAITEM_H__

#include <QtCore/QList>
#include <QtCore/QVariant>
#include <utils/ObjectWrapper.h>
#include <corelib/HierarchyHelper.h>
#include <corelib/HierarchyItem.h>
#include <corelib/IHierarchyDataItem.h>

namespace core {

class CORELIB_EXPORT HierarchyDataItem : public HierarchyItem, public IHierarchyDataItem
{
public:
    HierarchyDataItem(utils::ObjectWrapperConstPtr wrapper,
        const QIcon& icon, const QString& name, HierarchyHelperPtr helper);

    HierarchyDataItem(utils::ObjectWrapperConstPtr wrapper,
        const QIcon& icon, const QString& name, std::list<HierarchyHelperPtr> helpers);

    HierarchyDataItem(utils::ObjectWrapperConstPtr wrapper, 
        const QIcon& icon, const QString& name);

    HierarchyDataItem(utils::ObjectWrapperConstPtr wrapper, 
        const QIcon& icon = QIcon());

    HierarchyDataItem(const QIcon& icon, const QString& name, 
        HierarchyHelperPtr helper);

	virtual ~HierarchyDataItem();

public:
    virtual std::list<HierarchyHelperPtr> getHelpers() const { return helpers; }

    virtual utils::ObjectWrapperConstPtr getData() const;

private:
    std::list<HierarchyHelperPtr> helpers;
    utils::ObjectWrapperConstPtr data;
};
}

#endif
