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
 *	Item s�u��cy do podgl�du kamery. 
 */
class MultiViewCameraItem : public core::MultiView::Item
{
private:
    //! Kamera.
    osg::ref_ptr<osg::Camera> camera;
    //!
    osg::observer_ptr<osg::View> view;
    //! Wsp�czynnik proporcji.
    osgWidget::point_type aspectRatio;

    //! Indeks w grupie (rodzicu).
    unsigned childIdx;
    //! Rodzic w�z�a.
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

    //! \return Wsp�czynnik proporcji.
    virtual osgWidget::point_type getAspectRatio();
    //! \param aspectRatio Wsp�czynnik proporcji.
    void setDefaultAspectRatio(osgWidget::point_type aspectRatio);

    //! \return Widok na podstawie kt�rego okre�lany jest wsp�czynnik proporcji. Gdy NULL
    //!         to brany jest pod uwag� wsp�czynnik podawany jawnie.
    osg::View* getView()
    { 
        return view.get();
    }
    //! \param view Widok na podstawie kt�rego okre�lany jest wsp�czynnik proporcji. Gdy NULL
    //!             to brany jest pod uwag� wsp�czynnik podawany jawnie.
    void setView(osg::View* view) 
    { 
        this->view = view; 
    }

    //! Ustawia viewport kamery. Koryguje macierz projekcji. Implementacja wzorowana na
    //! osg::GraphicsContext::resizedImplementation.
    virtual void setLocation(osgWidget::point_type x, osgWidget::point_type y,
        osgWidget::point_type w, osgWidget::point_type h);


    //! Dodaje b�d� usuwa widget podgl�du ze sceny.
    virtual void setSelected(bool selected);

};

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__MULTIVIEWCAMERAITEM_H__