/********************************************************************
	created:	2013/02/10
	created:	10:2:2013   20:39
	filename: 	IVisualSourceNode.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__IVISUALSOURCENODE_H__
#define HEADER_GUARD_NEWVDF__IVISUALSOURCENODE_H__

#include <plugins/newVdf/IVisualNode.h>
#include <plugins/newVdf/IVisualOutputPin.h>

namespace vdf {

class IVisualOutputPin;
class IVisualInputPin;
class IVisualSourceNode : public virtual IVisualNode
{
public:
    virtual void addOutputPin(IVisualOutputPinPtr pin) = 0;
    virtual void removeOutputPin(IVisualOutputPinPtr pin) = 0;
	virtual void clearOutputPins() = 0;
	virtual int getNumOutputPins() const = 0;
	virtual IVisualOutputPinPtr getOutputPin(int no) = 0;
	virtual Type getType() const { return IVisualItem::SourceNode; }
	virtual bool isType(Type t) const { return t == IVisualItem::SourceNode || t == IVisualItem::Node; }
};
typedef core::shared_ptr<IVisualSourceNode> IVisualSourceNodePtr;
typedef core::shared_ptr<const IVisualSourceNode> IVisualSourceNodeConstPtr;

}

#endif	//	HEADER_GUARD___IVISUALNODE_H__
