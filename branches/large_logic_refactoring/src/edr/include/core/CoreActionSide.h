/********************************************************************
    created:  2013/01/18
    created:  18:1:2013   17:02
    filename: CoreActionSide.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___COREACTIONSIDE_H__
#define HEADER_GUARD___COREACTIONSIDE_H__

#include "CoreTitleBar.h"

namespace plugin {

class ICoreTitleBarSide
{
public:

	virtual const CoreTitleBar::SideType side() const = 0;

};

class IFlexiTabBarGroup {

public:

	virtual const QString group() const = 0;

};

}

#endif	//	HEADER_GUARD___COREACTIONSIDE_H__
