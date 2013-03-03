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

class XProcessor : public df::ProcessingNode, public df::IDFProcessor
{
public:
    XProcessor();

public:
    virtual void reset();
    virtual void process();

private:
    XInputPin * inPinA;
    XInputPin * inPinB;
    XOutputPin * outPinA;
    std::string name;
};

class VectorAdder : public df::ProcessingNode, public df::IDFProcessor
{
public:
    VectorAdder();

public:
    virtual void reset();
    virtual void process();

private:
    XInputPin * inPinA;
    XInputPin * inPinB;
    XOutputPin * outPinA;
    std::string name;
};

#endif
