/********************************************************************
	created:	2012/12/16
	created:	16:12:2012   19:34
	filename: 	NodesFactory.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__NODESFACTORY_H__
#define HEADER_GUARD_NEWVDF__NODESFACTORY_H__

#include <plugins/newVdf/INodesFactory.h>

class NodesFactory : public INodesFactory
{
public:
	virtual ~NodesFactory() {}

    virtual IVisualSinkNodePtr createSinkNode() const;
    virtual IVisualSourceNodePtr createSourceNode() const;
    virtual IVisualProcessingNodePtr createProcessingNode() const;
    virtual const QUuid id() const;
    virtual const QString name() const;
    virtual const QString description() const;
};
typedef core::shared_ptr<NodesFactory> NodesFactoryPtr;
typedef core::shared_ptr<const NodesFactory> NodesFactoryConstPtr;

//class StyleNodesFactory : public INodesFactory
//{
//public:
//	virtual ~StyleNodesFactory() {}
//
//	virtual IVisualSinkNode * createSinkNode() const;
//
//	virtual IVisualSourceNode * createSourceNode() const;
//
//	virtual IVisualProcessingNode * createProcessingNode() const;
//
//	virtual const QUuid id() const;
//
//	virtual const QString name() const;
//
//	virtual const QString description() const;
//
//};
//typedef core::shared_ptr<StyleNodesFactory> StyleNodesFactoryPtr;
//typedef core::shared_ptr<const StyleNodesFactory> StyleNodesFactoryConstPtr;

#endif
