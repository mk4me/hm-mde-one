/********************************************************************
    created:  2012/11/15
    created:  15:11:2012   10:15
    filename: IVisualItem.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IVISUALITEM_H__
#define HEADER_GUARD___IVISUALITEM_H__

#include <QtGui/QGraphicsItem>
#include <plugins/newVdf/IVisualStrategy.h>
//#include "HACK.h"

namespace vdf {

class IVisualItem
{
public:
	enum Type 
	{
		Pin,
		InputPin,
		OutputPin,
		Node,
		SinkNode,
		SourceNode,
		ProcessingNode,
		Connection,
		Custom
	};

public:
	virtual ~IVisualItem() {}

public:
	virtual QGraphicsItem * visualItem() const = 0;
	virtual void setVisualStrategy(IVisualStrategyPtr strategy) = 0;
	virtual Type getType() const = 0;
	virtual bool isType(Type t) const = 0;
};
typedef core::shared_ptr<IVisualItem> IVisualItemPtr;
typedef core::shared_ptr<const IVisualItem> IVisualItemConstPtr;

}

#endif	//	HEADER_GUARD___IVISUALITEM_H__
