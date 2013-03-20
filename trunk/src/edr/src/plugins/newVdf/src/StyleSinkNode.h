/********************************************************************
	created:	2013/02/01
	created:	1:2:2013   13:32
	filename: 	StyleSinkNode.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__STYLESINKNODE_H__
#define HEADER_GUARD_NEWVDF__STYLESINKNODE_H__

#include <QtGui/QGraphicsItem>
#include <QtCore/QSize>
#include <plugins/newVdf/IVisualSinkNode.h>
#include <plugins/newVdf/INodeStrategy.h>

namespace vdf {

class StyleItem;
class IVisualInputPin;

class StyleSinkNode : public IVisualSinkNode
{
public:
    StyleSinkNode();
    virtual ~StyleSinkNode() {}

public:
    virtual void addInputPin( IVisualInputPinPtr pin );
    virtual void removeInputPin( IVisualInputPinPtr pin );
    virtual void clearInputPins();
    virtual void setName( const QString & name );
	virtual QString getName() const;
    virtual void setConfigButton( QAbstractButton * button );
    virtual void setCloseButton( QAbstractButton * button );
    virtual QGraphicsItem * visualItem() const;
    virtual void addSelection();
    virtual void removeSelection();
    virtual void addHover();
    virtual void removeHover();
    virtual void addCollision();
    virtual void removeCollision();

	virtual int getNumInputPins() const { return pins.size(); }
	virtual IVisualInputPinPtr getInputPin(int no) { return pins[no]; }

	virtual void setVisualStrategy( IVisualStrategyPtr strategy );

    virtual df::INode* getModelNode() { return _node.get(); }
    virtual const df::INode* getModelNode() const { return _node.get(); }

    virtual const QIcon& getIcon() const { return icon; }
    virtual void setIcon(const QIcon& val) { icon = val; }

    virtual void setModelNode(df::INode* node) { _node.reset(node); }

    virtual Type getType() const { return IVisualItem::SinkNode; }
    virtual bool isType(Type t) const { return t == IVisualItem::SinkNode || t == IVisualItem::Node; }


 private:
 	core::shared_ptr<df::INode> _node;
    QIcon icon;
    StyleItem* styleItem;
    std::vector<IVisualInputPinPtr> pins;
	INodeStrategyPtr strategy;
	QString name;
};

}

#endif
