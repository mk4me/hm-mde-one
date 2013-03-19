/********************************************************************
    created:  2012/11/16
    created:  16:11:2012   9:35
    filename: IPinsFactory.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IPINSFACTORY_H__
#define HEADER_GUARD___IPINSFACTORY_H__

#include <plugins/newVdf/IFactory.h>
#include <plugins/newVdf/IVisualInputPin.h>
#include <plugins/newVdf/IVisualOutputPin.h>

namespace vdf {

class IPinsFactory : public IFactory
{
public:
	virtual IVisualInputPinPtr createInputPin() const = 0;
	virtual IVisualOutputPinPtr createOutputPin() const = 0;
};
typedef core::shared_ptr<IPinsFactory> IPinsFactoryPtr;
typedef core::shared_ptr<const IPinsFactory> IPinsFactoryConstPtr;

}

#endif	//	HEADER_GUARD___IPINSFACTORY_H__
