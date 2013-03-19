/********************************************************************
	created:	2013/02/01
	created:	1:2:2013   13:32
	filename: 	StyleMergedNode.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__STYLEMERGEDNODE_H__
#define HEADER_GUARD_NEWVDF__STYLEMERGEDNODE_H__

#include <QtGui/QGraphicsItem>
#include <QtCore/QSize>
#include <plugins/newVdf/IVisualNode.h>

namespace vdf {

class SimpleTextItem;
class IVisualInputPin;
class IVisualOutputPin;

class SimpleMergedNode : public IVisualProcessingNode
{
public:
    SimpleProcessingNode();
    virtual ~SimpleProcessingNode() {}

public:
    virtual void addInputPin( IVisualInputPin* pin );
    virtual void removeInputPin( IVisualInputPin* pin );
    virtual void clearInputPins();
    virtual void addOutputPin( IVisualOutputPin * pin );
    virtual void removeOutputPin( IVisualOutputPin* pin );
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

private:
    SimpleTextItem* simpleItem;
    std::vector<IVisualInputPin *> inputPins;
    std::vector<IVisualOutputPin *> outputPins;
};

}

#endif
