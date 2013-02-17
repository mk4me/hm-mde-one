/********************************************************************
	created:	2013/02/10
	created:	10:2:2013   20:37
	filename: 	IVisualProcessingNode.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__IVISUALPROCESSINGNODE_H__
#define HEADER_GUARD_NEWVDF__IVISUALPROCESSINGNODE_H__

#include <plugins/newVdf/IVisualSourceNode.h>
#include <plugins/newVdf/IVisualSinkNode.h>

class IVisualProcessingNode : public IVisualSinkNode, public IVisualSourceNode
{
public:
	virtual Type getType() const { return IVisualItem::ProcessingNode; }
	virtual bool isType(Type t) const { return t == IVisualItem::ProcessingNode || t == IVisualItem::Node; }
};
typedef core::shared_ptr<IVisualProcessingNode> IVisualProcessingNodePtr;
typedef core::shared_ptr<const IVisualProcessingNode> IVisualProcessingNodeConstPtr;

#endif	//	HEADER_GUARD___IVISUALNODE_H__
