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
 *	Item służący do podglądu kamery. 
 */
class MultiViewCameraItem : public core::MultiViewItem
{
private:
    //! Kamera.
    osg::ref_ptr<osg::Camera> camera;
    //!
    osg::observer_ptr<osg::View> view;
    //! Współczynnik proporcji.
    osgWidget::point_type aspectRatio;
    //! Indeks w grupie (rodzicu).
    unsigned childIdx;
    //! Rodzic węzła.
    osg::ref_ptr<osg::Group> parent;

public:
    //! \param camera
    //! \param aspectRatio
    MultiViewCameraItem(osg::Camera* camera, osg::Group* parent, osgWidget::point_type aspectRatio = 1);
    //! 
    //! \param camera
    //! \param view
    MultiViewCameraItem(osg::Camera* camera, osg::Group* parent, osg::View* view);

public:
    //! \return Nazwa widgetu.
    virtual const std::string& getName() const;

    //! \return Współczynnik proporcji.
    virtual osgWidget::point_type getAspectRatio();
    //! \param aspectRatio Współczynnik proporcji.
    void setDefaultAspectRatio(osgWidget::point_type aspectRatio);

    //! \return Widok na podstawie którego określany jest współczynnik proporcji. Gdy NULL
    //!         to brany jest pod uwagę współczynnik podawany jawnie.
    osg::View* getView()
    { 
        return view.get();
    }
    //! \param view Widok na podstawie którego określany jest współczynnik proporcji. Gdy NULL
    //!             to brany jest pod uwagę współczynnik podawany jawnie.
    void setView(osg::View* view) 
    { 
        this->view = view; 
    }

    //! Ustawia viewport kamery. Koryguje macierz projekcji. Implementacja wzorowana na
    //! osg::GraphicsContext::resizedImplementation.
    virtual void setLocation(osgWidget::point_type x, osgWidget::point_type y,
        osgWidget::point_type w, osgWidget::point_type h);


    //! Dodaje bądź usuwa widget podglądu ze sceny.
    virtual void setVisible(bool visible);
    //! Pomocnicza metoda do określania widoczności.
    bool isVisible() const;

};

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__MULTIVIEWCAMERAITEM_H__
