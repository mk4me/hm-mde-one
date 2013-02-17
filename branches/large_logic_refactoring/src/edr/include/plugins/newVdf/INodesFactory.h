/********************************************************************
    created:  2012/11/16
    created:  16:11:2012   9:36
    filename: INodesFactory.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___INODESFACTORY_H__
#define HEADER_GUARD___INODESFACTORY_H__

#include <plugins/newVdf/IFactory.h>
#include <plugins/newVdf/IVisualSinkNode.h>
#include <plugins/newVdf/IVisualSourceNode.h>
#include <plugins/newVdf/IVisualProcessingNode.h>

class INodesFactory : public IFactory
{
public:
	virtual IVisualSinkNodePtr createSinkNode() const = 0;
	virtual IVisualSourceNodePtr createSourceNode() const = 0;
	virtual IVisualProcessingNodePtr createProcessingNode() const = 0;
};
typedef core::shared_ptr<INodesFactory> INodesFactoryPtr;
typedef core::shared_ptr<const INodesFactory> INodesFactoryConstPtr;

#endif	//	HEADER_GUARD___INODESFACTORY_H__
