/********************************************************************
	created:	2013/02/28
	created:	28:2:2013   18:31
	filename: 	HmmProcessors.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__HMMPROCESSORS_H__
#define HEADER_GUARD_HMM__HMMPROCESSORS_H__

#include "HmmPins.h"

class XProcessor : public df::ProcessingNode, public df::IDFProcessor, public vdf::INodeConfiguration
{
public:

    XProcessor();

    virtual void reset();

    virtual void process();

    virtual QWidget* getConfigurationWidget() const;

    virtual void refreshConfiguration();

private:
    XInputPin * inPinA;
    XInputPin * inPinB;
    XOutputPin * outPinA;

    std::string name;

};

#endif
