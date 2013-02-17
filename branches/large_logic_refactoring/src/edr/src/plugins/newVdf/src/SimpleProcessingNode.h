/********************************************************************
	created:	2012/12/20
	created:	20:12:2012   11:44
	filename: 	SimpleProcessingNode.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__SIMPLEPROCESSINGNODE_H__
#define HEADER_GUARD_NEWVDF__SIMPLEPROCESSINGNODE_H__

#include <QtGui/QGraphicsItem>
#include <QtCore/QSize>
#include <plugins/newVdf/IVisualProcessingNode.h>

class SimpleTextItem;
class IVisualInputPin;
class IVisualOutputPin;

class SimpleProcessingNode : public IVisualProcessingNode
{
public:
    SimpleProcessingNode();
    virtual ~SimpleProcessingNode() {}

public:
    virtual void addInputPin( IVisualInputPinPtr pin );
    virtual void removeInputPin( IVisualInputPinPtr pin );
    virtual void clearInputPins();
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

	virtual int getNumInputPins() const { return inputPins.size(); }
	virtual IVisualInputPinPtr getInputPin(int no) { return inputPins[no]; }
	virtual int getNumOutputPins() const { return outputPins.size(); }
	virtual IVisualOutputPinPtr getOutputPin(int no) { return outputPins[no]; }
	virtual void setVisualStrategy( IVisualStrategyPtr strategy )
	{
		throw std::exception("The method or operation is not implemented.");
	}

private:
    SimpleTextItem* simpleItem;
    std::vector<IVisualInputPinPtr> inputPins;
    std::vector<IVisualOutputPinPtr> outputPins;
	QString name;
};

#endif
