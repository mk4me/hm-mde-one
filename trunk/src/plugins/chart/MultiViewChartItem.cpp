#include "MultiViewChartItem.h"
////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////
MultiViewChartItem::MultiViewChartItem(Chart* chart):
chart(chart)
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

void MultiViewChartItem::setSelected( bool selected )
{
	if ( selected ) {
       
    } else {
        
    }
}

void MultiViewChartItem::setLocation( osgWidget::point_type x, osgWidget::point_type y, osgWidget::point_type w, osgWidget::point_type h )
{
chart->setLocation(x, y, x+w,y+h);
}
////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////