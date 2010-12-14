#include "CorePCH.h"
#include <core/MultiViewFunctorWidgetAdapter.h>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

void MultiViewFunctorWidgetAdapter::setLabel( osgWidget::Widget* widget, const std::string& label )
{
    if ( setLabelFunc ) {
        setLabelFunc(widget, label);
    }
}

void MultiViewFunctorWidgetAdapter::setToggle( osgWidget::Widget* widget, bool toggle )
{
    if ( setToggleFunc ) {
        setToggleFunc(widget, toggle);
    }
}

osgWidget::EventInterface* MultiViewFunctorWidgetAdapter::getEventSource( osgWidget::Widget* widget )
{
    if (getEventSourceFunc) {
        return getEventSourceFunc(widget);
    }
    return NULL;
}

MultiView::WidgetAdapter* MultiViewFunctorWidgetAdapter::clone()
{
    return new MultiViewFunctorWidgetAdapter(*this);
}

osgWidget::point_type MultiViewFunctorWidgetAdapter::getMargin( osgWidget::Widget* widget )
{
    if (getMarginFunc) {
        return getMarginFunc(widget);
    }
    return WidgetAdapter::getMargin(widget);
}

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////