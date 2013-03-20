/********************************************************************
	created:	2013/02/01
	created:	1:2:2013   13:33
	filename: 	StyleSourceNode.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__STYLESOURCENODE_H__
#define HEADER_GUARD_NEWVDF__STYLESOURCENODE_H__

#include <QtGui/QGraphicsItem>
#include <QtCore/QSize>
#include <plugins/newVdf/IVisualSourceNode.h>
#include <plugins/newVdf/INodeStrategy.h>

namespace vdf {

class StyleItem;
class IVisualOutputPin;

class StyleSourceNode : public IVisualSourceNode
{
public:
    StyleSourceNode();
    virtual ~StyleSourceNode() {}

public:
    virtual void addOutputPin( IVisualOutputPinPtr pin );
    virtual void removeOutputPin( IVisualOutputPinPtr pin );
    virtual void clearOutputPins();
	virtual QString getName() const;
    virtual void setName( const QString & name );
    virtual void setConfigButton( QAbstractButton * button );
    virtual void setCloseButton( QAbstractButton * button );
    virtual QGraphicsItem * visualItem() const;
    virtual void addSelection();
    virtual void removeSelection();
    virtual void addHover();
    virtual void removeHover();
    virtual void addCollision();
    virtual void removeCollision();

	virtual int getNumOutputPins() const { return pins.size(); }
	virtual IVisualOutputPinPtr getOutputPin(int no) { return pins[no]; }

	virtual void setVisualStrategy( IVisualStrategyPtr strategy );

    virtual df::INode* getModelNode() { return _node.get(); }
    virtual const df::INode* getModelNode() const { return _node.get(); }

    virtual const QIcon& getIcon() const { return icon; }
    virtual void setIcon(const QIcon& val) { icon = val; }

    virtual void setModelNode(df::INode* node) { _node.reset(node); }
    virtual Type getType() const { return IVisualItem::SourceNode; }
    virtual bool isType(Type t) const { return t == IVisualItem::SourceNode || t == IVisualItem::Node; }


private:
    core::shared_ptr<df::INode> _node;
    QIcon icon;
    StyleItem* styleItem;
    std::vector<IVisualOutputPinPtr> pins;
	INodeStrategyPtr strategy;
	QString name;
};

}

#endif
