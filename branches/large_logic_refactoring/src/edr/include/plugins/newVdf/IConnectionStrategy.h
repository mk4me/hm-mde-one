/********************************************************************
	created:	2013/02/10
	created:	10:2:2013   20:17
	filename: 	IConnectionStrategy.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__ICONNECTIONSTRATEGY_H__
#define HEADER_GUARD_NEWVDF__ICONNECTIONSTRATEGY_H__

#include <plugins/newVdf/IVisualStrategy.h>

namespace vdf {

class IVisualPin;
class IConnectionStrategy : public IVisualStrategy
{
public:
	virtual void setPins(core::weak_ptr<IVisualPin> pin1, core::weak_ptr<IVisualPin> pin2) = 0;
};
typedef core::shared_ptr<IConnectionStrategy> IConnectionStrategyPtr;
typedef core::shared_ptr<const IConnectionStrategy> IConnectionStrategyConstPtr;

}
#endif
