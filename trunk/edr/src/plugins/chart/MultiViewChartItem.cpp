#include "MultiViewChartItem.h"
#include <osg/Notify>
////////////////////////////////////////////////////////////////////////////////
namespace core {
	////////////////////////////////////////////////////////////////////////////////
	MultiViewChartItem::MultiViewChartItem(Chart* chart,osg::Switch* multiViewSwitch):
chart(chart),
multiViewSwitch(multiViewSwitch)
{
	UTILS_ASSERT(chart);
}
const std::string& MultiViewChartItem::getName() const
{
	UTILS_ASSERT(chart != NULL);
	return chart->getName();
}

osgWidget::point_type MultiViewChartItem::getAspectRatio()
{
	return 3;
}

void MultiViewChartItem::setVisible( bool selected )
{
	multiViewSwitch->setValue(multiViewSwitch->getChildIndex(chart),selected);

}

void MultiViewChartItem::setLocation( osgWidget::point_type x, osgWidget::point_type y, osgWidget::point_type w, osgWidget::point_type h )
{


	chart->setLocation( static_cast<int>(x+0.5f),  static_cast<int>(y+0.5f),  static_cast<int>(x+w+0.5f), static_cast<int>(y+h+0.5f));
}
////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////