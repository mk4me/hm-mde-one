/********************************************************************
	created:	2012/12/20
	created:	20:12:2012   12:07
	filename: 	SimpleSinkNode.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__SIMPLESINKNODE_H__
#define HEADER_GUARD_NEWVDF__SIMPLESINKNODE_H__

#include <QtGui/QGraphicsItem>
#include <QtCore/QSize>
#include <plugins/newVdf/IVisualSinkNode.h>

class SimpleTextItem;
class IVisualInputPin;

class SimpleSinkNode : public IVisualSinkNode
{
public:
    SimpleSinkNode();
    virtual ~SimpleSinkNode() {}

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

private:
    SimpleTextItem* simpleItem;
    std::vector<IVisualInputPinPtr> pins;
	QString name;
};


#endif
