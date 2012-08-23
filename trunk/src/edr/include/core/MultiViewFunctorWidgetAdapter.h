/********************************************************************
	created:  2010/12/03
	created:  3:12:2010   16:24
	filename: MultiViewFunctorWidgetAdapter.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__MULTIVIEWFUNCTORWIDGETADAPTER_H__
#define __HEADER_GUARD__MULTIVIEWFUNCTORWIDGETADAPTER_H__

#include <boost/function.hpp>
#include <core/MultiView.h>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

/**
 *	Przyk³adowa, najprotsza implementacja adaptera. Bazuje na obiektach boost::function.
 *  U¿ywany jest przy definicji domyœlnych styli, ale mo¿na go wykorzystaæ równie¿ w kodzie
 *  klienckim.
 */
struct MultiViewFunctorWidgetAdapter : public MultiView::WidgetAdapter
{
public:
    boost::function< void (osgWidget::Widget*, const std::string& ) > setLabelFunc;
    boost::function< void (osgWidget::Widget*, bool) > setToggleFunc;
    boost::function< osgWidget::EventInterface* (osgWidget::Widget*) > getEventSourceFunc;
    boost::function< osgWidget::point_type (osgWidget::Widget*) > getMarginFunc;

    virtual MultiView::WidgetAdapter* clone();
    virtual osgWidget::EventInterface* getEventSource(osgWidget::Widget* widget);
    virtual void setLabel(osgWidget::Widget* widget, const std::string& label);
    virtual void setToggle(osgWidget::Widget* widget, bool toggle);
    virtual osgWidget::point_type getMargin(osgWidget::Widget* widget);
};

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__MULTIVIEWFUNCTORWIDGETADAPTER_H__