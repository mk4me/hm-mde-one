/********************************************************************
	created:	2013/02/28
	created:	28:2:2013   18:31
	filename: 	HmmSinks.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__HMMSINKS_H__
#define HEADER_GUARD_HMM__HMMSINKS_H__

#include <plugins/dfElements/DFPins.h>
#include <QtGui/QLineEdit>
#include "TreeItemHelper.h"

class HmmMainWindow;

//! Klasa potrafi obs³u¿yæ piny, które implementuj¹ interfejs IMDEOutputPin
class XInputPin : public df::InputPin, public df::IDFInput
{
public:
    //! 
    //! \param node 
    XInputPin( df::ISinkNode * node );

public:
    //! 
    virtual void reset();
    //! 
    //! \param pin 
    void copyData( const df::IDFOutput * pin );
    //! 
    //! \param pin 
    virtual const bool pinCompatible( const df::IOutputPin * pin ) const;
    //! 
    utils::ObjectWrapperConstPtr getWrapper() const;
    //! 
    //! \param val 
    void setWrapper(utils::ObjectWrapperConstPtr val);

private:
    utils::ObjectWrapperConstPtr wrapper;
};

class XSink : public df::SinkNode, public df::IDFSink, public vdf::INodeConfiguration
{
public:
    XSink();
    XSink(HmmMainWindow* h);
    
    virtual void reset();
    virtual void consume();

    void insertToTree( TreeItemHelperPtr channelHelper);//, utils::ObjectWrapperPtr wrp );

    HmmMainWindow* getHmm() const;
    void setHmm(HmmMainWindow* val);

    virtual QWidget* getConfigurationWidget();
    virtual void refreshConfiguration();

private:
    void _XSink();

private:
    HmmMainWindow* hmm;
    XInputPin * inPinA;
    QWidget* widget;
    QLineEdit* line;
    QString defaultName;
    int runNo;
};


#endif
