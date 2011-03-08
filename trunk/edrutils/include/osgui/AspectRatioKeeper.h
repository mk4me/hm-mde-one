/********************************************************************
	created:  2010/11/23
	created:  23:11:2010   20:08
	filename: AspectRatioKeeperWindow.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__ASPECTRATIOKEEPER_H__
#define __HEADER_GUARD__ASPECTRATIOKEEPER_H__

#include <osgWidget/Window>
#include <osgui/macroHacks.h>

////////////////////////////////////////////////////////////////////////////////
namespace osgUI {
////////////////////////////////////////////////////////////////////////////////

class AspectRatioKeeper : public osgWidget::Window::EmbeddedWindow
{
public:
    //META_Object(osgUI, AspectRatioKeeper);
	META_Widget(osgUI, AspectRatioKeeper);
    //! \param widget
    //! \param aspectRatio
    AspectRatioKeeper(osgWidget::Widget* widget = NULL, osgWidget::point_type aspectRatio = 1);
    //!
    AspectRatioKeeper(const AspectRatioKeeper& keeper, const osg::CopyOp& copyop);

public:
    //! \return
    osgWidget::Widget* getTarget();
    //! \return
    const osgWidget::Widget* getTarget() const;
    //! \param target
    void setTarget(osgWidget::Widget* target);
    //! \return
    osgWidget::point_type getAspectRatio() const;
    //! \param aspectRatio
    void setAspectRatio(osgWidget::point_type aspectRatio);
};

////////////////////////////////////////////////////////////////////////////////
} // namespace osgUI
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__ASPECTRATIOKEEPER_H__