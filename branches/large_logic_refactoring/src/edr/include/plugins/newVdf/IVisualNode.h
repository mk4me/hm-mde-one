/********************************************************************
    created:  2012/11/15
    created:  15:11:2012   9:48
    filename: IVisualNode.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IVISUALNODE_H__
#define HEADER_GUARD___IVISUALNODE_H__

//#include <dflib/Node.h>
#include <plugins/newVdf/IVisualPin.h>
#include <QtGui/QAbstractButton>
#include <utils/Debug.h>


class IVisualNode : public IVisualItem
{
protected:
	IVisualNode() : _node(nullptr) {}

public:
	virtual ~IVisualNode() {}

public:
	virtual void setName(const QString & name) = 0;
	virtual QString getName() const = 0;
	virtual void setConfigButton(QAbstractButton * button) = 0;
	virtual void setCloseButton(QAbstractButton * button) = 0;
	// TODO: przeniesc implementacje
	df::INode* getModelNode() {  UTILS_ASSERT(_node); return _node; }
	const df::INode* getModelNode() const { UTILS_ASSERT(_node); return _node; }
	void setModelNode(df::INode* node) { _node = node; }

	virtual void addSelection() = 0;
	virtual void removeSelection() = 0;

private:
	df::INode* _node;
};
typedef core::shared_ptr<IVisualNode> IVisualNodePtr;
typedef core::weak_ptr<IVisualNode> IVisualNodeWeakPtr;
typedef core::shared_ptr<const IVisualNode> IVisualNodeConstPtr;

//class IVisualSourceNode : public virtual IVisualNode
//{
//public:
//
//    virtual void addOutputPin(IVisualOutputPin * pin) = 0;
//    virtual void removeOutputPin(IVisualOutputPin* pin) = 0;
//	virtual void clearOutputPins() = 0;
//	virtual int getNumOutputPins() const = 0;
//	virtual IVisualOutputPin* getOutputPin(int no) = 0;
//	virtual Type getType() const { return IVisualItem::SourceNode; }
//	virtual bool isType(Type t) const { return t == IVisualItem::SourceNode || t == IVisualItem::Node; }
//};
//
//class IVisualSinkNode : public virtual IVisualNode
//{
//public:
//    virtual void addInputPin(IVisualInputPin* pin) = 0;
//    virtual void removeInputPin(IVisualInputPin* pin) = 0;
//	virtual void clearInputPins() = 0;
//
//	virtual int getNumInputPins() const = 0;
//	virtual IVisualInputPin* getInputPin(int no) = 0;
//	virtual Type getType() const { return IVisualItem::SinkNode; }
//	virtual bool isType(Type t) const { return t == IVisualItem::SinkNode || t == IVisualItem::Node; }
//};
//
//class IVisualProcessingNode : public IVisualSinkNode, public IVisualSourceNode
//{
//public:
//	virtual Type getType() const { return IVisualItem::ProcessingNode; }
//	virtual bool isType(Type t) const { return t == IVisualItem::ProcessingNode || t == IVisualItem::Node; }
//};

#endif	//	HEADER_GUARD___IVISUALNODE_H__
