/********************************************************************
	created:	2012/12/16
	created:	16:12:2012   19:34
	filename: 	DarkNodesFactory.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__DARKNODESFACTORY_H__
#define HEADER_GUARD_NEWVDF__DARKNODESFACTORY_H__

#include <plugins/newVdf/INodesFactory.h>

class DarkNodesFactory : public INodesFactory
{
public:
	virtual ~DarkNodesFactory() {}

public:
    virtual IVisualSinkNodePtr createSinkNode() const;
	virtual IVisualSourceNodePtr createSourceNode() const;
    virtual IVisualProcessingNodePtr createProcessingNode() const;
    virtual const QUuid id() const;
    virtual const QString name() const;
    virtual const QString description() const;
};
typedef core::shared_ptr<DarkNodesFactory> DarkNodesFactoryPtr;
typedef core::shared_ptr<const DarkNodesFactory> DarkNodesFactoryConstPtr;

#endif
