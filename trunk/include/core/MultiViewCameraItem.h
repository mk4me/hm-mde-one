/********************************************************************
	created:  2010/12/13
	created:  13:12:2010   12:58
	filename: MultiViewCameraItem.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__MULTIVIEWCAMERAITEM_H__
#define __HEADER_GUARD__MULTIVIEWCAMERAITEM_H__

#include <osg/ref_ptr>
#include <osg/observer_ptr>
#include <osg/Camera>
#include <osg/View>
#include <osg/Viewport>
#include <core/MultiView.h>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

/**
 *	Item s³u¿¹cy do podgl¹du kamery. 
 */
class MultiViewCameraItem : public core::MultiView::Item
{
private:
    //! Kamera.
    osg::ref_ptr<osg::Camera> camera;
    //!
    osg::observer_ptr<osg::View> view;
    //! Wspó³czynnik proporcji.
    osgWidget::point_type aspectRatio;

    //! Indeks w grupie (rodzicu).
    unsigned childIdx;
    //! Rodzic wêz³a.
    osg::observer_ptr<osg::Group> parent;

public:
    //! \param camera
    //! \param aspectRatio
    MultiViewCameraItem(osg::Camera* camera, osgWidget::point_type aspectRatio = 1);
    //! 
    //! \param camera
    //! \param view
    MultiViewCameraItem(osg::Camera* camera, osg::View* view);

public:
    //! \return Nazwa widgetu.
    virtual const std::string& getName() const;

    //! \return Wspó³czynnik proporcji.
    virtual osgWidget::point_type getAspectRatio();
    //! \param aspectRatio Wspó³czynnik proporcji.
    void setDefaultAspectRatio(osgWidget::point_type aspectRatio);

    //! \return Widok na podstawie którego okreœlany jest wspó³czynnik proporcji. Gdy NULL
    //!         to brany jest pod uwagê wspó³czynnik podawany jawnie.
    osg::View* getView()
    { 
        return view.get();
    }
    //! \param view Widok na podstawie którego okreœlany jest wspó³czynnik proporcji. Gdy NULL
    //!             to brany jest pod uwagê wspó³czynnik podawany jawnie.
    void setView(osg::View* view) 
    { 
        this->view = view; 
    }

    //! Ustawia viewport kamery. Koryguje macierz projekcji. Implementacja wzorowana na
    //! osg::GraphicsContext::resizedImplementation.
    virtual void setLocation(osgWidget::point_type x, osgWidget::point_type y,
        osgWidget::point_type w, osgWidget::point_type h);


    //! Dodaje b¹dŸ usuwa widget podgl¹du ze sceny.
    virtual void setSelected(bool selected);

};

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__MULTIVIEWCAMERAITEM_H__