/********************************************************************
	created:	2013/02/10
	created:	10:2:2013   20:27
	filename: 	IBackgroundsFactory.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__IBACKGROUNDSFACTORY_H__
#define HEADER_GUARD_NEWVDF__IBACKGROUNDSFACTORY_H__

#include <plugins/newVdf/IFactory.h>
#include <plugins/newVdf/IBackgroundStrategy.h>

namespace vdf {

class IBackgroundsFactory : public IFactory
{	
public:
	virtual ~IBackgroundsFactory() {}

	virtual IBackgroundStrategyPtr createBackgroundStrategy() = 0;
};
typedef core::shared_ptr<IBackgroundsFactory> IBackgroundsFactoryPtr;
typedef core::shared_ptr<const IBackgroundsFactory> IBackgroundsFactoryConstPtr;
}

#endif
