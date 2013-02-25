/********************************************************************
	created:	2012/12/20
	created:	20:12:2012   11:09
	filename: 	SimpleOutputPinNode.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__SIMPLEOUTPUTPINNODE_H__
#define HEADER_GUARD_NEWVDF__SIMPLEOUTPUTPINNODE_H__

#include <QtGui/QGraphicsItem>
#include <QtCore/QSize>
#include <plugins/newVdf/IVisualOutputPin.h>
#include <plugins/newVdf/IVisualConnection.h>
#include <plugins/newVdf/IVisualNode.h>

namespace vdf {

class SimplePinItem;

class SimpleOutputPinNode : public IVisualOutputPin
{
public:
    SimpleOutputPinNode();
    virtual ~SimpleOutputPinNode();

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
	virtual void addConnection(IVisualConnectionWeakPtr connection) { connections.push_back(connection); }
    virtual void removeConnection(core::weak_ptr<IVisualConnection> connection);
	virtual IVisualConnectionWeakPtr getConnection(int idx) const { return connections[idx]; }
    virtual int getNumConnections() const { return connections.size(); }

	virtual void setVisualStrategy( IVisualStrategyPtr strategy );

protected:
	IVisualNodeWeakPtr parent;
	std::vector<IVisualConnectionWeakPtr> connections;
	int index;
private:
    SimplePinItem* item;
};

}

#endif
