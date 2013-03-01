/********************************************************************
	created:	2013/02/28
	created:	28:2:2013   18:31
	filename: 	HmmSources.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__HMMSOURCES_H__
#define HEADER_GUARD_HMM__HMMSOURCES_H__

#include "HmmPins.h"
#include <utils/ObjectWrapper.h>
#include <QtGui/QTreeWidget>

class XSource : public df::SourceNode, public df::IDFSource, public vdf::INodeConfiguration
{
public:
    XSource ();
    
public:
    virtual void reset();
    virtual const bool empty() const;
    virtual void produce();
    virtual QWidget* getConfigurationWidget() const;

public slots:
    void itemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

private:
    virtual void refreshConfiguration();

private:
    XOutputPin * outPinA;
    QTreeWidget* tree;
    VectorChannelReaderInterfaceConstPtr channel;
    bool used;
    std::map<QTreeWidgetItem*, utils::ObjectWrapperConstPtr> item2wrapper;
};


//class XSource : public df::SourceNode, public df::IDFSource
//{
//public:
//
//    XSource ();
//
//    XSource(VectorChannelReaderInterfaceConstPtr vector);
//
//    virtual void reset();
//
//    virtual const bool empty() const;
//
//    virtual void produce();
//
//    VectorChannelReaderInterfaceConstPtr getChannel() const;
//    void setChannel(VectorChannelReaderInterfaceConstPtr val);
//
//
//private:
//    void _XSource();
//
//private:
//    XOutputPin * outPinA;
//    VectorChannelReaderInterfaceConstPtr channel;
//    bool used;
//};


#endif
