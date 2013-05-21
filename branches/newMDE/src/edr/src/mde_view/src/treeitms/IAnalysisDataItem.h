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
#include "TreeItemHelper.h"

class IAnalysisDataItem
{
public:
	virtual ~IAnalysisDataItem() {}

public:
    virtual TreeItemHelperPtr getHelper() const = 0;
    //virtual utils::ObjectWrapperConstPtr getData() const = 0;
};
DEFINE_SMART_POINTERS(IAnalysisDataItem);

#endif
