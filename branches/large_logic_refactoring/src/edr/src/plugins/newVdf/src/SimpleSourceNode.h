/********************************************************************
	created:	2012/12/20
	created:	20:12:2012   12:20
	filename: 	SimpleSourceNode.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__SIMPLESOURCENODE_H__
#define HEADER_GUARD_NEWVDF__SIMPLESOURCENODE_H__

#include <QtGui/QGraphicsItem>
#include <QtCore/QSize>
#include <plugins/newVdf/IVisualSourceNode.h>

class SimpleTextItem;
class IVisualOutputPin;

class SimpleSourceNode : public IVisualSourceNode
{
public:
    SimpleSourceNode();
    virtual ~SimpleSourceNode() {}

public:
    virtual void addOutputPin( IVisualOutputPinPtr pin );
    virtual void removeOutputPin( IVisualOutputPinPtr pin );
    virtual void clearOutputPins();
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

	virtual int getNumOutputPins() const { return pins.size(); }
	virtual IVisualOutputPinPtr getOutputPin(int no) { return pins[no]; }

	virtual void setVisualStrategy( IVisualStrategyPtr strategy );


private:
    SimpleTextItem* simpleItem;
    std::vector<IVisualOutputPinPtr> pins;
	QString name;
};

#endif
