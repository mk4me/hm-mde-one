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

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

/**
 *	Item b�d�cy widgetem. Trzeba pami�ta� �e widget/okno trzeba doda� do
 *  window managera OSOBNO! Mo�na go u�ywa� zar�wno jako Item oraz PreviewItem.
 */
class MultiViewWidgetItem : public MultiView::PreviewItem
{
private:
    //! Pe�en podgl�d.
    osg::ref_ptr<osgWidget::Window> window;
    //! Wsp�czynnik proporcji.
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
    
    //! \return Okno zwi�zane z itemem.
    osgWidget::Window* getWindow()
    { 
        return window;
    }

    //!
    bool isSelected();

// MultiView::PreviewItem
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