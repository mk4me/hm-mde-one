/********************************************************************
	created:	2013/02/01
	created:	1:2:2013   13:32
	filename: 	StyleOutputPinNode.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__STYLEOUTPUTPINNODE_H__
#define HEADER_GUARD_NEWVDF__STYLEOUTPUTPINNODE_H__


#include <QtGui/QGraphicsItem>
#include <QtCore/QSize>
#include <plugins/newVdf/IVisualOutputPin.h>
#include <plugins/newVdf/IPinStrategy.h>
#include <plugins/newVdf/IVisualNode.h>
#include <plugins/newVdf/IVisualConnection.h>

namespace vdf {

class StyleItem;

class StyleOutputPinNode : public IVisualOutputPin
{
public:
    StyleOutputPinNode();
    virtual ~StyleOutputPinNode();

public:
    virtual void markIncomplete();
    virtual void markNormal();
    virtual void markCompatible();
    virtual void markIncompatible();
    virtual void markConnective();
    virtual void markUnconnective();
    virtual void markConnected();
    virtual QGraphicsItem * visualItem() const;
    virtual void addSelection();
    virtual void removeSelection();
    virtual void addHover();
    virtual void removeHover();
    virtual void addCollision();
    virtual void removeCollision();
	virtual int getIndex() const { UTILS_ASSERT(index != -1); return index; }
	virtual void setIndex(int val) { index = val; }
	IVisualNodeWeakPtr getParent() const { return parent; }
	void setParent(IVisualNodeWeakPtr val) { parent = val; }
	virtual void addConnection(IVisualConnectionWeakPtr connection);
    virtual void removeConnection(IVisualConnectionWeakPtr connection);
	virtual IVisualConnectionWeakPtr getConnection(int idx) const { return connections[idx]; }
    virtual int getNumConnections() const { return connections.size(); }

	virtual void setVisualStrategy( IVisualStrategyPtr strategy );

    virtual df::IOutputPin* getModelPin() const;

protected:
	IVisualNodeWeakPtr parent;
	std::vector<IVisualConnectionWeakPtr> connections;
	IPinStrategyPtr strategy;
	int index;
    StyleItem* item;
};

}

#endif
