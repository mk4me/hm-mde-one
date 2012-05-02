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
 *	Item bêd¹cy widgetem. Trzeba pamiêtaæ ¿e widget/okno trzeba dodaæ do
 *  window managera OSOBNO! Mo¿na go u¿ywaæ zarówno jako Item oraz Item.
 */
class MultiViewWidgetItem : public MultiViewItem
{
private:
    //! Pe³en podgl¹d.
    osg::ref_ptr<osgWidget::Window> window;
    //! Wspó³czynnik proporcji.
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
    
    //! \return Okno zwi¹zane z itemem.
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
    //! \return Wspó³czynnik proporcji.
    virtual osgWidget::point_type getAspectRatio();
    //! Dodaje b¹dŸ usuwa widget podgl¹du ze sceny.
    virtual void setVisible(bool visible);
    //! Umieszcza widget w zadanej lokalizacji.
    virtual void setLocation(osgWidget::point_type x, osgWidget::point_type y,
        osgWidget::point_type w, osgWidget::point_type h);

};

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////



#endif  // __HEADER_GUARD__MULTIVIEWWIDGETITEM_H__