#include "CorePCH.h"
#include <utils/Debug.h>
#include <core/MultiViewWidgetItem.h>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

MultiViewWidgetItem::MultiViewWidgetItem( osgWidget::Window* window, osgWidget::point_type aspectRatio ) :
window(window),
aspectRatio(aspectRatio)
{
    UTILS_ASSERT(window);
}

MultiViewWidgetItem::MultiViewWidgetItem( osgWidget::Widget* widget, osgWidget::point_type aspectRatio ) :
aspectRatio(aspectRatio)
{
    UTILS_ASSERT(widget);
    // tworzymy wrapper
    osgWidget::Box* box = new osgWidget::Box(widget->getName());
    box->addWidget(widget);
    window = box;
}

const std::string& MultiViewWidgetItem::getName() const
{
    UTILS_ASSERT(window != NULL);
    return window->getName();
}

osgWidget::point_type MultiViewWidgetItem::getAspectRatio()
{
    return aspectRatio;
}

void MultiViewWidgetItem::setVisible( bool visible )
{
    if ( visible ) {
        window->show();
    } else {
        window->hide();
    }
}

void MultiViewWidgetItem::setLocation( osgWidget::point_type x, osgWidget::point_type y, osgWidget::point_type w, osgWidget::point_type h )
{
    window->setOrigin(x, y);
    window->update();
    window->resize(w, h);
}

bool MultiViewWidgetItem::isSelected()
{
    return window->isVisible();
}

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////
