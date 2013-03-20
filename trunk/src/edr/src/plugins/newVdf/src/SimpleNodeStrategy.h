/********************************************************************
	created:	2013/03/14
	created:	14:3:2013   20:57
	filename: 	SimpleNodeStrategy.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__SIMPLENODESTRATEGY_H__
#define HEADER_GUARD_NEWVDF__SIMPLENODESTRATEGY_H__

#include <plugins/newVdf/INodeStrategy.h>
#include <QtCore/QRect>

namespace vdf {

class SimpleNodeStrategy : public INodeStrategy
{
public:
	SimpleNodeStrategy();

	void setContainerStyle(INodeStrategy::State state);

	virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr );
	virtual void setNode(core::weak_ptr<IVisualNode> item);
	virtual const QRectF& getRect();
	virtual QWidget* getWidget();
	virtual void update();
	virtual QPointF getPinPosition( int no, bool input );
	virtual void setState( State state );

private:
	core::weak_ptr<IVisualNode> node;
	QRectF rect;
	int height;
	std::string name;
};
typedef core::shared_ptr<SimpleNodeStrategy> SimpleNodeStrategyPtr;
typedef core::shared_ptr<const SimpleNodeStrategy> SimpleNodeStrategyConstPtr;
}

#endif
