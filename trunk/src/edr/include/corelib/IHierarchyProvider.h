/********************************************************************
	created:  2016/07/29 13:07
	filename: IHierarchyProvider.h
	author:	  Wojciech Knie�
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IHIERARCHYPROVIDER_H__
#define HEADER_GUARD___IHIERARCHYPROVIDER_H__

#include <corelib/IHierarchyItem.h>

namespace core {

class IHierarchyProvider
{
public:
	virtual ~IHierarchyProvider() {}

public:
	virtual void initHierarchyProvider(core::IHierarchyItemPtr parent) = 0;
	virtual void disconnectedFromHierarchy() = 0;
};
DEFINE_SMART_POINTERS(IHierarchyProvider);

}
#endif