/********************************************************************
	created:	2013/02/28
	created:	28:2:2013   19:08
	filename: 	HmmPins.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__HMMPINS_H__
#define HEADER_GUARD_HMM__HMMPINS_H__

#include <plugins/newVdf/NewVdfService.h>
#include <plugins/newVdf/INodeConfiguration.h>
#include <plugins/c3d/C3DChannels.h>

class HmmMainWindow;

class XOutputPin : public df::OutputPin, public df::IDFOutput
{
public:
    XOutputPin(df::ISourceNode * node);

    const VectorChannelReaderInterfaceConstPtr value() const;
    void value(VectorChannelReaderInterfaceConstPtr val);

    virtual void reset();

private:
    VectorChannelReaderInterfaceConstPtr val;
};

class XInputPin : public df::InputPin, public df::IDFInput
{
public:
    XInputPin(df::ISinkNode * node);

public:
    virtual void copyData(const df::IDFOutput * pin);
    virtual void reset();
    const VectorChannelReaderInterfaceConstPtr value() const;

private:
    VectorChannelReaderInterfaceConstPtr val;
};


#endif
