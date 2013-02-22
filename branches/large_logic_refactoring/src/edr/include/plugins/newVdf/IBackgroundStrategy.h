/********************************************************************
	created:	2013/02/10
	created:	10:2:2013   20:18
	filename: 	IBackgroundStrategy.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__IBACKGROUNDSTRATEGY_H__
#define HEADER_GUARD_NEWVDF__IBACKGROUNDSTRATEGY_H__

#include <corelib/SmartPtr.h>

namespace vdf {

class IBackgroundStrategy
{
public:
	virtual ~IBackgroundStrategy() {}
	virtual void paint(QPainter*, const QRectF&) = 0;
};
typedef core::shared_ptr<IBackgroundStrategy> IBackgroundStrategyPtr;
typedef core::shared_ptr<const IBackgroundStrategy> IBackgroundStrategyConstPtr;

}
#endif
