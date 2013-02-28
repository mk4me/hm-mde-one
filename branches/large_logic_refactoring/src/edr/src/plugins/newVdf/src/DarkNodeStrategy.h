/********************************************************************
	created:	2013/02/07
	created:	7:2:2013   13:20
	filename: 	DarkNodeStrategy.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__DARKNODESTRATEGY_H__
#define HEADER_GUARD_NEWVDF__DARKNODESTRATEGY_H__

#include <plugins/newVdf/INodeStrategy.h>
#include <QtCore/QRect>

namespace vdf {

class DarkNodeStrategy : public INodeStrategy
{
public:
	DarkNodeStrategy();

	void setContainerStyle(INodeStrategy::State state);

	virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr ) {}
	virtual void setNode(core::weak_ptr<IVisualNode> item);
	virtual const QRectF& getRect();
	virtual QWidget* getWidget();
	virtual void update();
	virtual QPointF getPinPosition( int no, bool input );
	virtual void setState( State state );

private:
	core::weak_ptr<IVisualNode> node;
	QLabel* label;
	QWidget* container;
	QLabel* lowerFrame;
	QRectF rect;
	int labelHeight;
};

typedef core::shared_ptr<DarkNodeStrategy> DarkNodeStrategyPtr;
typedef core::shared_ptr<const DarkNodeStrategy> DarkNodeStrategyConstPtr;

}

#endif
