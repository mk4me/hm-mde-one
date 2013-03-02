/********************************************************************
    created:  2012/11/15
    created:  15:11:2012   9:48
    filename: IVisualNode.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IVISUALNODE_H__
#define HEADER_GUARD___IVISUALNODE_H__

#include <plugins/newVdf/IVisualPin.h>
#include <QtGui/QAbstractButton>
#include <utils/Debug.h>

namespace df { class INode; }

namespace vdf {

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
    const QIcon& getIcon() const { return icon; }
    void setIcon(const QIcon& val) { icon = val; }

	void setModelNode(df::INode* node) { _node = node; }

	virtual void addSelection() = 0;
	virtual void removeSelection() = 0;

private:
	df::INode* _node;
    QIcon icon;
};
typedef core::shared_ptr<IVisualNode> IVisualNodePtr;
typedef core::weak_ptr<IVisualNode> IVisualNodeWeakPtr;
typedef core::shared_ptr<const IVisualNode> IVisualNodeConstPtr;

}

#endif	//	HEADER_GUARD___IVISUALNODE_H__
