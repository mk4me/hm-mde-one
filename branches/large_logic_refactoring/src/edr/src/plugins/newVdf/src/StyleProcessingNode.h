/********************************************************************
	created:	2013/02/01
	created:	1:2:2013   13:32
	filename: 	StyleProcessingNode.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__STYLEPROCESSINGNODE_H__
#define HEADER_GUARD_NEWVDF__STYLEPROCESSINGNODE_H__

#include <QtGui/QGraphicsItem>
#include <QtCore/QSize>
#include <plugins/newVdf/IVisualProcessingNode.h>
#include <plugins/newVdf/INodeStrategy.h>

class StyleItem;
class IVisualInputPin;
class IVisualOutputPin;

class StyleProcessingNode : public IVisualProcessingNode
{
public:
    StyleProcessingNode();
    virtual ~StyleProcessingNode() {}

public:
    virtual void addInputPin( IVisualInputPinPtr pin );
    virtual void removeInputPin( IVisualInputPinPtr pin );
    virtual void clearInputPins();
    virtual void addOutputPin( IVisualOutputPinPtr pin );
    virtual void removeOutputPin( IVisualOutputPinPtr pin );
    virtual void clearOutputPins();

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

	virtual int getNumInputPins() const { return inputPins.size(); }
	virtual IVisualInputPinPtr getInputPin(int no) { return inputPins[no]; }
	virtual int getNumOutputPins() const { return outputPins.size(); }
	virtual IVisualOutputPinPtr getOutputPin(int no) { return outputPins[no]; }

	virtual void setVisualStrategy( IVisualStrategyPtr strategy );

	virtual QString getName() const;

private:
    StyleItem* styleItem;
    std::vector<IVisualInputPinPtr> inputPins;
    std::vector<IVisualOutputPinPtr> outputPins;
	INodeStrategyPtr strategy;
	QString name;
};

#endif
