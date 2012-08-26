/********************************************************************
	created:  2010/12/13
	created:  13:12:2010   14:24
	filename: MultiViewManipulatorItem.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__MULTIVIEWMANIPULATORITEM_H__
#define __HEADER_GUARD__MULTIVIEWMANIPULATORITEM_H__

#include <osg/Camera>
#include <osgViewer/Viewer>
#include <osgGA/CameraManipulator>
#include <core/MultiView.h>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

//! Item powinien być używany wyłącznie jako preview. Jego działanie polega
//! na kontrolowaniu manipulatorów kamer.
class MultiViewManipulatorItem : public core::MultiViewItem
{
private:
    //! Kamera z miniaturka.
    osg::ref_ptr<osg::Camera> thumbnailCamera;
    //! Współczynnik proporcji.
    osg::observer_ptr<osgViewer::View> view;
    //! Manipulator kamery.
    osg::ref_ptr<osgGA::CameraManipulator> manipulator;

public:
    //! \param camera
    //! \param aspectRatio
    //! \param view
    //! \param manipulator
    MultiViewManipulatorItem(osg::Camera* thumbnailCamera, osgViewer::View* view, osgGA::CameraManipulator* manipulator);

    //! \return Nazwa widgetu.
    virtual const std::string& getName() const;

    //! \return Współczynnik proporcji.
    virtual osgWidget::point_type getAspectRatio();

    //! Ustawia/usuwa manipulator viewera oraz przenosi transformację z/do kamery.
    virtual void setVisible(bool visible);

    //! Tutaj nic się nie dzieje - pracujemy na głównej kamerze viewera, która
    //! zawsze zajmuje pełny ekran.
    virtual void setLocation(osgWidget::point_type x, osgWidget::point_type y,
        osgWidget::point_type w, osgWidget::point_type h)
    {}
};

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__MULTIVIEWMANIPULATORITEM_H__
