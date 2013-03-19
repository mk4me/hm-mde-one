/********************************************************************
	created:	2013/02/10
	created:	10:2:2013   20:38
	filename: 	IVisualSinkNode.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__IVISUALSINKNODE_H__
#define HEADER_GUARD_NEWVDF__IVISUALSINKNODE_H__

#include <plugins/newVdf/IVisualInputPin.h>
#include <plugins/newVdf/IVisualNode.h>
#include <plugins/newVdf/IVisualInputPin.h>

namespace vdf {

class IVisualSinkNode : public virtual IVisualNode
{
public:
    virtual void addInputPin(IVisualInputPinPtr pin) = 0;
    virtual void removeInputPin(IVisualInputPinPtr pin) = 0;
	virtual void clearInputPins() = 0;

	virtual int getNumInputPins() const = 0;
	virtual IVisualInputPinPtr getInputPin(int no) = 0;
	virtual Type getType() const { return IVisualItem::SinkNode; }
	virtual bool isType(Type t) const { return t == IVisualItem::SinkNode || t == IVisualItem::Node; }
};
typedef core::shared_ptr<IVisualSinkNode> IVisualSinkNodePtr;
typedef core::shared_ptr<const IVisualSinkNode> IVisualSinkNodeConstPtr;

}

#endif	//	HEADER_GUARD___IVISUALNODE_H__
