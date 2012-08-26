/********************************************************************
	created:  2010/11/26
	created:  26:11:2010   21:25
	filename: MultiViewWidgetItem.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__MULTIVIEWWIDGETITEM_H__
#define __HEADER_GUARD__MULTIVIEWWIDGETITEM_H__

#include <osgWidget/Window>
#include <osgWidget/Widget>
#include <core/MultiView.h>
#include <osgui/QOsgWidgets.h>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

/**
 *	Item będący widgetem. Trzeba pamiętać że widget/okno trzeba dodać do
 *  window managera OSOBNO! Można go używać zarówno jako Item oraz Item.
 */
class MultiViewWidgetItem : public MultiViewItem
{
private:
    //! Pełen podgląd.
    osg::ref_ptr<osgWidget::Window> window;
    //! Współczynnik proporcji.
    osgWidget::point_type aspectRatio;

public:
    //! \param window
    //! \param manager
    //! \param aspectRatio
    MultiViewWidgetItem(osgWidget::Window* window, osgWidget::point_type aspectRatio );

    //! \param window
    //! \param manager
    //! \param aspectRatio
    MultiViewWidgetItem(osgWidget::Widget* widget, osgWidget::point_type aspectRatio);
    
    //! \return Okno związane z itemem.
    osgWidget::Window* getWindow()
    { 
        return window;
    }

    //!
    bool isSelected();

// MultiViewItem
public:
    //! \return Nazwa widgetu.
    virtual const std::string& getName() const;
    //! \return Współczynnik proporcji.
    virtual osgWidget::point_type getAspectRatio();
    //! Dodaje bądź usuwa widget podglądu ze sceny.
    virtual void setVisible(bool visible);
    //! Umieszcza widget w zadanej lokalizacji.
    virtual void setLocation(osgWidget::point_type x, osgWidget::point_type y,
        osgWidget::point_type w, osgWidget::point_type h);

};

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////



#endif  // __HEADER_GUARD__MULTIVIEWWIDGETITEM_H__
