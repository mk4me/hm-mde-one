/********************************************************************
	created:	2013/02/28
	created:	28:2:2013   18:31
	filename: 	HmmSinks.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__HMMSINKS_H__
#define HEADER_GUARD_HMM__HMMSINKS_H__

#include "HmmPins.h"

class XSink : public df::SinkNode, public df::IDFSink
{
public:
    XSink();
    XSink(HmmMainWindow* h);
    
    virtual void reset();
    virtual void consume();
    HmmMainWindow* getHmm() const;
    void setHmm(HmmMainWindow* val);

private:
    void _XSink();

private:
    HmmMainWindow* hmm;
    XInputPin * inPinA;
};


#endif
