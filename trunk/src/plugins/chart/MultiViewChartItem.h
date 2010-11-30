/********************************************************************
	created:  2010/11/29
	created:  26:11:2010   14:25
	filename: MultiViewItem.h
	author:	  Micha� Szafarski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__MULTIVIEWCHARTITEM_H__
#define __HEADER_GUARD__MULTIVIEWCHARTITEM_H__

#include <osgWidget/Window>
#include <osgWidget/Widget>
#include <core/MultiView.h>
#include <core/Chart.h>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////


class MultiViewChartItem : public MultiView::PreviewItem
{
private:
	std::string name;
	Chart* chart;
public:
	MultiViewChartItem(Chart* chart);
	

public:
    //! \return Nazwa widgetu.
    virtual const std::string& getName() const;
    //! \return Wsp�czynnik proporcji.
    virtual osgWidget::point_type getAspectRatio();
    //! Dodaje b�d� usuwa widget podgl�du ze sceny.
    virtual void setSelected(bool selected);
    //! Umieszcza widget w zadanej lokalizacji.
    virtual void setLocation(osgWidget::point_type x, osgWidget::point_type y,
        osgWidget::point_type w, osgWidget::point_type h);
};

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__MULTIVIEWWIDGETITEM_H__