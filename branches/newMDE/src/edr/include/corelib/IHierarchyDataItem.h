/********************************************************************
	created:	2013/04/18
	created:	18:4:2013   9:22
	filename: 	IAnalysisDataItem.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_MDE_VIEW__IANALYSISDATAITEM_H__
#define HEADER_GUARD_MDE_VIEW__IANALYSISDATAITEM_H__

#include <utils/ObjectWrapper.h>
#include <corelib/HierarchyHelper.h>

namespace core {

class IHierarchyDataItem
{
public:
	virtual ~IHierarchyDataItem() {}

public:
    virtual std::list<HierarchyHelperPtr> getHelpers() const = 0;
    virtual utils::ObjectWrapperConstPtr getData() const = 0;
};
DEFINE_SMART_POINTERS(IHierarchyDataItem);

}

#endif
